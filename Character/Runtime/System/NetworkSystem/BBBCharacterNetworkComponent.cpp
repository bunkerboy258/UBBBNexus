#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBAimStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBEquipmentStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBLocomotionStatePacket.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UBBBCharacterNetworkComponent::UBBBCharacterNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBCharacterNetworkComponent::Initialize(ABBBCharacter &InCharacter)
{
    Character = &InCharacter;
    ReplicatedFactLedger.Initialize(*this);
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 本机控制角色已经生成同一份事实 只让模拟代理执行接收投递
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedFactLedger, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedEquipmentId, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAimState, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLocomotionState, COND_SimulatedOnly);
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipmentFact_Implementation(FBBBEquipmentActionFact Fact)
{
    // 服务端不重新仲裁玩法 只阻止结构损坏的事实进入角色输入系统
    if (!ensureMsgf(Character && IsOwnerAuthority() && Fact.Sequence > 0 && Fact.EquipmentId != NAME_None,
        TEXT("[UBBBC]Equipment fact delivery was rejected")))
    {
        return;
    }

    SubmitEquipmentFactInput(Fact);
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipmentState_Implementation(const FName EquipmentId)
{
    // 装备状态没有空标识时才具备创建镜像装备的语义
    if (!ensureMsgf(Character && IsOwnerAuthority() && EquipmentId != NAME_None,
        TEXT("[UBBBC]Equipment state delivery was rejected")))
    {
        return;
    }

    SubmitEquipmentStateInput(EquipmentId);
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{
    // 连续状态允许覆盖 但非法向量不能污染角色黑板
    if (!ensureMsgf(Character && IsOwnerAuthority() && !FVector(AimState.AimTargetWorld).ContainsNaN(),
        TEXT("[UBBBC]Aim state delivery was rejected")))
    {
        return;
    }

    SubmitAimStateInput(AimState);
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(
    FBBBLocomotionNetworkState LocomotionState)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(), TEXT("[UBBBC]Locomotion state delivery was rejected")))
    {
        return;
    }

    SubmitLocomotionStateInput(LocomotionState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedEquipmentId()
{
    // OnRep 与 RPC 共用投递边界 保证接收来源不影响领域应用规则
    SubmitEquipmentStateInput(ReplicatedEquipmentId);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    SubmitAimStateInput(ReplicatedAimState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedLocomotionState()
{
    SubmitLocomotionStateInput(ReplicatedLocomotionState);
}

void UBBBCharacterNetworkComponent::ReplicateEquipmentFact(FBBBEquipmentActionFact Fact)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish equipment facts")))
    {
        return;
    }

    // 账本条目必须先标脏 再请求本演员尽快进入复制调度
    ReplicatedFactLedger.Append(MoveTemp(Fact));
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ReplicateEquipmentState(const FName EquipmentId)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish equipment state")))
    {
        return;
    }

    ReplicatedEquipmentId = EquipmentId;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ReplicateAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish aim state")))
    {
        return;
    }

    ReplicatedAimState = AimState;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ReplicateLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish locomotion state")))
    {
        return;
    }

    ReplicatedLocomotionState = LocomotionState;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::SubmitEquipmentFactInput(const FBBBEquipmentActionFact &Fact)
{
    if (!ensureMsgf(Character, TEXT("[UBBBC]Equipment fact arrived without a character")))
    {
        return;
    }

    // 此处是离散事实从网络格式进入领域输入格式的唯一翻译点
    switch (Fact.PacketId)
    {
    case FBBBEquipFactPacket::PacketId:
    {
        const bool bAccepted = Character->SubmitInput(
            FBBBEquipFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Equipment fact input was rejected"));
        return;
    }
    case FBBBFireFactPacket::PacketId:
    {
        const bool bAccepted = Character->SubmitInput(
            FBBBFireFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Fire fact input was rejected"));
        return;
    }
    case FBBBReloadStartedFactPacket::PacketId:
    {
        const bool bAccepted = Character->SubmitInput(
            FBBBReloadStartedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Reload start fact input was rejected"));
        return;
    }
    case FBBBMagazineDetachedFactPacket::PacketId:
    {
        const bool bAccepted = Character->SubmitInput(
            FBBBMagazineDetachedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Magazine detach fact input was rejected"));
        return;
    }
    case FBBBMagazineLoadedFactPacket::PacketId:
    {
        const bool bAccepted = Character->SubmitInput(
            FBBBMagazineLoadedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Magazine load fact input was rejected"));
        return;
    }
    case FBBBReloadCancelledFactPacket::PacketId:
    {
        const bool bAccepted = Character->SubmitInput(
            FBBBReloadCancelledFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Reload cancel fact input was rejected"));
        return;
    }
    default:
        ensureMsgf(false, TEXT("[UBBBC]Unknown equipment fact packet id %d"), Fact.PacketId);
        return;
    }
}

void UBBBCharacterNetworkComponent::SubmitEquipmentStateInput(const FName EquipmentId)
{
    if (Character && EquipmentId != NAME_None)
    {
        const bool bAccepted = Character->SubmitInput(FBBBEquipmentStatePacket{EquipmentId});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Equipment state input was rejected"));
    }
}

void UBBBCharacterNetworkComponent::SubmitAimStateInput(const FBBBAimNetworkState &AimState)
{
    if (Character)
    {
        // 连续快照在边界转成领域状态包 之后由 ParseSystem 统一应用
        FBBBAimStatePacket Packet;
        Packet.State = FBBBAimRuntimeState{AimState.bIsAiming, AimState.AimTargetWorld};
        const bool bAccepted = Character->SubmitInput(MoveTemp(Packet));
        ensureMsgf(bAccepted, TEXT("[UBBBC]Aim state input was rejected"));
    }
}

void UBBBCharacterNetworkComponent::SubmitLocomotionStateInput(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (Character)
    {
        const bool bAccepted = Character->SubmitInput(FBBBLocomotionStatePacket{LocomotionState.Gait});
        ensureMsgf(bAccepted, TEXT("[UBBBC]Locomotion state input was rejected"));
    }
}

bool UBBBCharacterNetworkComponent::IsOwnerLocallyControlled() const
{
    const APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->IsLocallyControlled();
}

bool UBBBCharacterNetworkComponent::IsOwnerAuthority() const
{
    const APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->HasAuthority();
}

APawn *UBBBCharacterNetworkComponent::GetOwnerPawn() const
{
    APawn *OwnerPawn = Cast<APawn>(GetOwner());
    ensureMsgf(OwnerPawn, TEXT("[UBBBC]Network component owner is not APawn"));
    return OwnerPawn;
}
