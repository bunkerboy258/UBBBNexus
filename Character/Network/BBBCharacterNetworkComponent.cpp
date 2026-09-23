#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

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
    ReplicatedEquipmentFacts.Initialize(*this);
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 本机控制角色已经生成同一份事实 只让模拟代理执行接收投递
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedEquipmentFacts,
        COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedEquipmentId,
        COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedAimState,
        COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedGait,
        COND_SimulatedOnly);
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipmentFact_Implementation(
    FBBBEquipmentActionFact Fact)
{
    // 服务端不重新仲裁玩法 只阻止结构损坏的事实进入角色输入系统
    if (!ensureMsgf(
        Character && IsOwnerAuthority() && Fact.Sequence > 0 && Fact.EquipmentId != NAME_None,
        TEXT("装备事实网络投递被拒绝")))
    {
        return;
    }

    SubmitEquipmentFactInput(Fact);
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipmentState_Implementation(
    const FName EquipmentId)
{
    // 装备状态没有空标识时才具备创建镜像装备的语义
    if (!ensureMsgf(
        Character && IsOwnerAuthority() && EquipmentId != NAME_None,
        TEXT("装备状态网络投递被拒绝")))
    {
        return;
    }

    SubmitEquipmentStateInput(EquipmentId);
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(
    FBBBReplicatedAimState AimState)
{
    // 连续状态允许覆盖 但非法向量不能污染角色输入
    if (!ensureMsgf(
        Character
            && IsOwnerAuthority()
            && !FVector(AimState.AimTargetWorld).ContainsNaN()
            && FMath::IsFinite(AimState.AimAlpha)
            && AimState.AimAlpha >= 0.0f
            && AimState.AimAlpha <= 1.0f,
        TEXT("瞄准状态网络投递被拒绝")))
    {
        return;
    }

    SubmitAimStateInput(AimState);
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(
    const EBBBCharacterGait Gait)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(), TEXT("步态网络投递被拒绝")))
    {
        return;
    }

    SubmitLocomotionStateInput(Gait);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedEquipmentId()
{
    SubmitEquipmentStateInput(ReplicatedEquipmentId);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    SubmitAimStateInput(ReplicatedAimState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedGait()
{
    SubmitLocomotionStateInput(ReplicatedGait);
}

void UBBBCharacterNetworkComponent::ReplicateEquipmentFact(FBBBEquipmentActionFact Fact)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    // 装备事实先登记为增量再要求所属演员尽快进入复制调度
    ReplicatedEquipmentFacts.Append(MoveTemp(Fact));
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ReplicateEquipmentState(const FName EquipmentId)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    ReplicatedEquipmentId = EquipmentId;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ReplicateAimState(
    const FBBBReplicatedAimState &AimState)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    ReplicatedAimState = AimState;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ReplicateLocomotionState(
    const EBBBCharacterGait Gait)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    ReplicatedGait = Gait;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::SubmitEquipmentFactInput(
    const FBBBEquipmentActionFact &Fact)
{
    if (!Character)
    {
        return;
    }

    // 此处是装备事实从网络格式进入角色输入格式的唯一翻译点
    switch (Fact.Type)
    {
    case EBBBEquipmentActionType::Equip:
    {
        Character->SubmitInput(FBBBEquipFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    }
    case EBBBEquipmentActionType::Fire:
    {
        Character->SubmitInput(FBBBFireFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    }
    case EBBBEquipmentActionType::ReloadStarted:
    {
        Character->SubmitInput(FBBBReloadStartedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    }
    case EBBBEquipmentActionType::MagazineDetached:
    {
        Character->SubmitInput(FBBBMagazineDetachedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    }
    case EBBBEquipmentActionType::MagazineLoaded:
    {
        Character->SubmitInput(FBBBMagazineLoadedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    }
    case EBBBEquipmentActionType::ReloadCancelled:
    {
        Character->SubmitInput(FBBBReloadCancelledFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    }
    default:
        ensureMsgf(false, TEXT("收到无法识别的装备事实类型"));
        return;
    }
}

void UBBBCharacterNetworkComponent::SubmitEquipmentStateInput(const FName EquipmentId)
{
    if (Character && EquipmentId != NAME_None)
    {
        Character->SubmitInput(FBBBEquipmentStatePacket{EquipmentId});
    }
}

void UBBBCharacterNetworkComponent::SubmitAimStateInput(
    const FBBBReplicatedAimState &AimState)
{
    if (!Character)
    {
        return;
    }

    FBBBAimStatePacket Packet;
    Packet.bIsAiming = AimState.bIsAiming;
    Packet.AimAlpha = AimState.AimAlpha;
    Packet.AimTargetWorld = AimState.AimTargetWorld;
    Character->SubmitInput(MoveTemp(Packet));
}

void UBBBCharacterNetworkComponent::SubmitLocomotionStateInput(
    const EBBBCharacterGait Gait)
{
    if (Character)
    {
        Character->SubmitInput(FBBBLocomotionStatePacket{Gait});
    }
}

bool UBBBCharacterNetworkComponent::IsOwnerAuthority() const
{
    const APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->HasAuthority();
}

APawn *UBBBCharacterNetworkComponent::GetOwnerPawn() const
{
    return Cast<APawn>(GetOwner());
}
