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
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedFactLedger, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedEquipmentId, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAimState, COND_SimulatedOnly);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLocomotionState, COND_SimulatedOnly);
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipmentFact_Implementation(FBBBEquipmentActionFact Fact)
{
    if (!ensureMsgf(Character && IsOwnerAuthority() && Fact.Sequence > 0 && Fact.EquipmentId != NAME_None,
        TEXT("[UBBBC]Equipment fact delivery was rejected")))
    {
        return;
    }

    DeliverEquipmentFact(Fact);
}

void UBBBCharacterNetworkComponent::ServerSubmitEquipmentState_Implementation(const FName EquipmentId)
{
    if (!ensureMsgf(Character && IsOwnerAuthority() && EquipmentId != NAME_None,
        TEXT("[UBBBC]Equipment state delivery was rejected")))
    {
        return;
    }

    DeliverEquipmentState(EquipmentId);
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{
    if (!ensureMsgf(Character && IsOwnerAuthority() && !FVector(AimState.AimTargetWorld).ContainsNaN(),
        TEXT("[UBBBC]Aim state delivery was rejected")))
    {
        return;
    }

    DeliverAimState(AimState);
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(
    FBBBLocomotionNetworkState LocomotionState)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(), TEXT("[UBBBC]Locomotion state delivery was rejected")))
    {
        return;
    }

    DeliverLocomotionState(LocomotionState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedEquipmentId()
{
    DeliverEquipmentState(ReplicatedEquipmentId);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    DeliverAimState(ReplicatedAimState);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedLocomotionState()
{
    DeliverLocomotionState(ReplicatedLocomotionState);
}

void UBBBCharacterNetworkComponent::PublishEquipmentFact(FBBBEquipmentActionFact Fact)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish equipment facts")))
    {
        return;
    }

    ReplicatedFactLedger.Append(MoveTemp(Fact));
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::PublishEquipmentState(const FName EquipmentId)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish equipment state")))
    {
        return;
    }

    ReplicatedEquipmentId = EquipmentId;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::PublishAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish aim state")))
    {
        return;
    }

    ReplicatedAimState = AimState;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::PublishLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (!ensureMsgf(IsOwnerAuthority(), TEXT("[UBBBC]Only authority may publish locomotion state")))
    {
        return;
    }

    ReplicatedLocomotionState = LocomotionState;
    GetOwner()->ForceNetUpdate();
}

void UBBBCharacterNetworkComponent::DeliverEquipmentFact(const FBBBEquipmentActionFact &Fact)
{
    if (!ensureMsgf(Character, TEXT("[UBBBC]Equipment fact arrived without a character")))
    {
        return;
    }

    switch (Fact.PacketId)
    {
    case FBBBEquipFactPacket::PacketId:
        Character->SubmitInput(FBBBEquipFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    case FBBBFireFactPacket::PacketId:
        Character->SubmitInput(FBBBFireFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    case FBBBReloadStartedFactPacket::PacketId:
        Character->SubmitInput(FBBBReloadStartedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    case FBBBMagazineDetachedFactPacket::PacketId:
        Character->SubmitInput(FBBBMagazineDetachedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    case FBBBMagazineLoadedFactPacket::PacketId:
        Character->SubmitInput(FBBBMagazineLoadedFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    case FBBBReloadCancelledFactPacket::PacketId:
        Character->SubmitInput(FBBBReloadCancelledFactPacket{Fact.EquipmentId, Fact.Sequence, Fact.LoadedAmmo});
        return;
    default:
        ensureMsgf(false, TEXT("[UBBBC]Unknown equipment fact packet id %d"), Fact.PacketId);
        return;
    }
}

void UBBBCharacterNetworkComponent::DeliverEquipmentState(const FName EquipmentId)
{
    if (Character && EquipmentId != NAME_None)
    {
        Character->SubmitInput(FBBBEquipmentStatePacket{EquipmentId});
    }
}

void UBBBCharacterNetworkComponent::DeliverAimState(const FBBBAimNetworkState &AimState)
{
    if (Character)
    {
        Character->SubmitInput(FBBBAimStatePacket{
            FBBBAimRuntimeState{AimState.bIsAiming, AimState.AimTargetWorld}});
    }
}

void UBBBCharacterNetworkComponent::DeliverLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (Character)
    {
        Character->SubmitInput(FBBBLocomotionStatePacket{LocomotionState.Gait});
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
