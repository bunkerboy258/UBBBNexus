#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreEquipmentPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreLocomotionPacket.h"
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
    if (!Character || IsOwnerLocallyControlled())
    {
        return;
    }

    FBBBRestoreEquipmentPacket Restore;
    Restore.EquipmentHandle = Packet.EquipmentHandle;
    Character->SubmitInput(Restore);
}

void UBBBCharacterNetworkComponent::MulticastEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!Character || IsOwnerLocallyControlled())
    {
        return;
    }

    // PacketId 到还原事实包的唯一翻译点 线上协议与包类型在此对应
    switch (Packet.PacketId)
    {
    case FBBBFireFactPacket::PacketId:
    {
        FBBBFireFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Character->SubmitInput(Restore);
        break;
    }
    case FBBBReloadStartedFactPacket::PacketId:
    {
        FBBBReloadStartedFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Character->SubmitInput(Restore);
        break;
    }
    case FBBBMagazineDetachedFactPacket::PacketId:
    {
        FBBBMagazineDetachedFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Character->SubmitInput(Restore);
        break;
    }
    case FBBBMagazineLoadedFactPacket::PacketId:
    {
        FBBBMagazineLoadedFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Character->SubmitInput(Restore);
        break;
    }
    case FBBBReloadCancelledFactPacket::PacketId:
    {
        FBBBReloadCancelledFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Character->SubmitInput(Restore);
        break;
    }
    case FBBBEquipFactPacket::PacketId:
    {
        FBBBEquipFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Character->SubmitInput(Restore);
        break;
    }
    default:
        ensureMsgf(false, TEXT("[UBBBC]Unknown equipment action packet id %d"), Packet.PacketId);
        break;
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{
    SetReplicatedAimState(AimState);
    if (Character && !IsOwnerLocallyControlled())
    {
        FBBBRestoreAimPacket Restore;
        Restore.State = FBBBAimRuntimeState{AimState.bIsAiming, AimState.AimTargetWorld};
        Character->SubmitInput(Restore);
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(FBBBLocomotionNetworkState LocomotionState)
{
    SetReplicatedLocomotionState(LocomotionState);
    if (Character && !IsOwnerLocallyControlled())
    {
        FBBBRestoreLocomotionPacket Restore;
        Restore.Gait = LocomotionState.Gait;
        Character->SubmitInput(Restore);
    }
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    if (!Character)
    {
        return;
    }

    FBBBRestoreAimPacket Restore;
    Restore.State = FBBBAimRuntimeState{ReplicatedAimState.bIsAiming, ReplicatedAimState.AimTargetWorld};
    Character->SubmitInput(Restore);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedLocomotionState()
{
    if (!Character)
    {
        return;
    }

    FBBBRestoreLocomotionPacket Restore;
    Restore.Gait = ReplicatedLocomotionState.Gait;
    Character->SubmitInput(Restore);
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
