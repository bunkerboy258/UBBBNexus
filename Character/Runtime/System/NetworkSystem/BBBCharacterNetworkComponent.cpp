#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UBBBCharacterNetworkComponent::UBBBCharacterNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBCharacterNetworkComponent::Initialize(FBBBCharacterInput &InInput)
{
    Input = &InInput;
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAimState, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLocomotionState, COND_SkipOwner);
}

void UBBBCharacterNetworkComponent::ServerUploadEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    MulticastEquipmentPacket(MoveTemp(Packet));
}

void UBBBCharacterNetworkComponent::ServerUploadEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    MulticastEquipmentActionPacket(MoveTemp(Packet));
}

void UBBBCharacterNetworkComponent::MulticastEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    if (!Input || IsOwnerLocallyControlled())
    {
        return;
    }

    FBBBCharacterRestoreDiscreteInput Restore;
    Restore.EquipmentHandle = Packet.EquipmentHandle;
    Input->SubmitRestore(MoveTemp(Restore));
}

void UBBBCharacterNetworkComponent::MulticastEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!Input || IsOwnerLocallyControlled())
    {
        return;
    }

    FBBBCharacterRestoreDiscreteInput Restore;
    Restore.EquipmentEvent.ActionType = Packet.ActionType;
    Restore.EquipmentEvent.EquipmentId = Packet.EquipmentId;
    Restore.EquipmentEvent.Sequence = Packet.Sequence;
    Restore.EquipmentEvent.Phase = Packet.Phase;
    Restore.EquipmentEvent.LoadedAmmo = Packet.LoadedAmmo;
    Input->SubmitRestore(MoveTemp(Restore));
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{
    SetReplicatedAimState(AimState);
    if (Input && !IsOwnerLocallyControlled())
    {
        FBBBCharacterRestoreDiscreteInput Restore;
        Restore.Aim = FBBBAimRuntimeState{AimState.bIsAiming, AimState.AimTargetWorld};
        Input->SubmitRestore(MoveTemp(Restore));
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(FBBBLocomotionNetworkState LocomotionState)
{
    SetReplicatedLocomotionState(LocomotionState);
    if (Input && !IsOwnerLocallyControlled())
    {
        FBBBCharacterRestoreDiscreteInput Restore;
        Restore.Gait = LocomotionState.Gait;
        Input->SubmitRestore(MoveTemp(Restore));
    }
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    if (!Input)
    {
        return;
    }

    FBBBCharacterRestoreDiscreteInput Restore;
    Restore.Aim = FBBBAimRuntimeState{ReplicatedAimState.bIsAiming, ReplicatedAimState.AimTargetWorld};
    Input->SubmitRestore(MoveTemp(Restore));
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedLocomotionState()
{
    if (!Input)
    {
        return;
    }

    FBBBCharacterRestoreDiscreteInput Restore;
    Restore.Gait = ReplicatedLocomotionState.Gait;
    Input->SubmitRestore(MoveTemp(Restore));
}

void UBBBCharacterNetworkComponent::SetReplicatedAimState(const FBBBAimNetworkState &AimState)
{
    ReplicatedAimState = AimState;
}

void UBBBCharacterNetworkComponent::SetReplicatedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    ReplicatedLocomotionState = LocomotionState;
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
