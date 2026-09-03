#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"

#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UBBBCharacterNetworkComponent::UBBBCharacterNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBCharacterNetworkComponent::Initialize(FBBBCharacterNetworkSystem &InNetworkSystem)
{
    NetworkSystem = &InNetworkSystem;
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAimState, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedLocomotionState, COND_SkipOwner);
}

void UBBBCharacterNetworkComponent::ServerUploadEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment packet receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentForDistribution(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::ServerUploadEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment action packet receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentActionForDistribution(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::MulticastEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment multicast receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentForRestore(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::MulticastEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment action multicast receive failed because network system is null")))
    {
        NetworkSystem->ReceiveEquipmentActionForRestore(MoveTemp(Packet));
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Aim state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveSubmittedAimState(AimState);
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(FBBBLocomotionNetworkState LocomotionState)
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Locomotion state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveSubmittedLocomotionState(LocomotionState);
    }
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Replicated aim state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveReplicatedAimState(ReplicatedAimState);
    }
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedLocomotionState()
{
    if (ensureMsgf(NetworkSystem, TEXT("[UBBBC]Replicated locomotion state receive failed because network system is null")))
    {
        NetworkSystem->ReceiveReplicatedLocomotionState(ReplicatedLocomotionState);
    }
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
