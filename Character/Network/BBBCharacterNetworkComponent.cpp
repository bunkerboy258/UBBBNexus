#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBTraversalEndAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBTraversalStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Locomotion/FBBBTraversalEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Locomotion/FBBBTraversalStartRemoteMessagePacket.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Aim/FBBBAimStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBRunStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Equipment/FBBBEquipmentSelectionRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Equipment/FBBBEquipmentSelectionAuthorityFactPacket.h"
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
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedEquipmentId, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedEquipmentGeneration, COND_SimulatedOnly);

    // 本机控制角色已经生成同一份事实 只让模拟代理执行接收投递
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        ReplicatedAimState,
        COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(
        UBBBCharacterNetworkComponent,
        bReplicatedRun,
        COND_SimulatedOnly);
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(
    FBBBReplicatedAimState AimState)
{
    // 连续状态允许覆盖 但非法向量不能污染角色输入
    if (!ensureMsgf(
        Character
            && IsOwnerAuthority()
            && !FVector(AimState.AimTargetWorld).ContainsNaN(),
        TEXT("瞄准状态网络投递被拒绝")))
    {
        return;
    }

    SubmitAimStateInput(AimState);
}

void UBBBCharacterNetworkComponent::ServerSubmitRunState_Implementation(const bool bRun)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(), TEXT("跑步状态网络投递被拒绝")))
    {
        return;
    }

    SubmitRunStateInput(bRun);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    SubmitAimStateInput(ReplicatedAimState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedRunState()
{
    SubmitRunStateInput(bReplicatedRun);
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

void UBBBCharacterNetworkComponent::ReplicateRunState(const bool bRun)
{
    if (!IsOwnerAuthority())
    {
        return;
    }

    bReplicatedRun = bRun;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::SubmitAimStateInput(
    const FBBBReplicatedAimState &AimState)
{
    if (!Character)
    {
        return;
    }

    FBBBAimStateAuthorityFactPacket Packet;
    Packet.bIsAiming = AimState.bIsAiming;
    Packet.AimTargetWorld = AimState.AimTargetWorld;
    Character->SubmitInput(MoveTemp(Packet));
}

void UBBBCharacterNetworkComponent::SubmitRunStateInput(const bool bRun)
{
    if (Character)
    {
        Character->SubmitInput(FBBBRunStateAuthorityFactPacket{bRun});
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

void UBBBCharacterNetworkComponent::ReplicateTraversal(uint32 Id, EBBBTraversalAction Action,
    const FTransform &Contact, const FTransform &End)
{
    MulticastTraversal(Id, Action, Contact, End);
}

void UBBBCharacterNetworkComponent::ServerSubmitTraversal_Implementation(uint32 Id,
    EBBBTraversalAction Action, FTransform Contact, FTransform End)
{
    if (!Character || !IsOwnerAuthority() || Id == 0 || !Contact.IsValid() || !End.IsValid()
        || Action > EBBBTraversalAction::ClimbHigh)
    {
        return;
    }
    if (Action == EBBBTraversalAction::None)
    {
        Character->SubmitInput(FBBBTraversalEndRemoteMessagePacket{Id});
        return;
    }
    Character->SubmitInput(FBBBTraversalStartRemoteMessagePacket{Id, Action, Contact, End});
}

void UBBBCharacterNetworkComponent::MulticastTraversal_Implementation(uint32 Id,
    EBBBTraversalAction Action, FTransform Contact, FTransform End)
{
    if (!Character || IsOwnerAuthority() || Character->IsLocallyControlled())
    {
        return;
    }
    if (Action == EBBBTraversalAction::None)
    {
        Character->SubmitInput(FBBBTraversalEndAuthorityFactPacket{Id});
        return;
    }
    Character->SubmitInput(FBBBTraversalStartAuthorityFactPacket{Id, Action, Contact, End});
}

void UBBBCharacterNetworkComponent::ReplicateEquipment(const FName EquipmentId, const uint64 Generation)
{
    if (!IsOwnerAuthority() || Generation == 0 || Generation < ReplicatedEquipmentGeneration)
    {
        return;
    }

    ReplicatedEquipmentId = EquipmentId;
    ReplicatedEquipmentGeneration = Generation;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipment_Implementation(const FName EquipmentId, const uint64 Generation)
{
    if (Character && IsOwnerAuthority() && Generation > ReplicatedEquipmentGeneration)
    {
        Character->SubmitInput(FBBBEquipmentSelectionRemoteMessagePacket{{EquipmentId}, {Generation}});
    }
}

void UBBBCharacterNetworkComponent::OnRep_Equipment()
{
    if (Character && Character->IsNetworkMirror() && ReplicatedEquipmentGeneration > 0)
    {
        Character->SubmitInput(FBBBEquipmentSelectionAuthorityFactPacket{{ReplicatedEquipmentId}, {ReplicatedEquipmentGeneration}});
    }
}
