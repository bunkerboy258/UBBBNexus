#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterMovementPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBEquipSlotPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBFirePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBReloadPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Replication/BBBRestoreAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Replication/BBBRestoreEquipmentPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Replication/BBBRestoreLocomotionPacket.h"
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

void UBBBCharacterNetworkComponent::ServerSubmitControlPacket_Implementation(
    FBBBCharacterControlNetworkPacket Packet)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(),
        TEXT("[UBBBC]Control command arrived without an authoritative character")))
    {
        return;
    }

    if (Packet.bMovementActive)
    {
        FBBBCharacterMovementPacket Movement;
        Movement.MoveWorld = Packet.MoveWorld;
        Movement.FacingWorld = Packet.FacingWorld;
        Movement.bWalk = Packet.bWalk;
        Movement.bSprint = Packet.bSprint;
        Movement.bCrouch = Packet.bCrouch;
        Character->SubmitInput(MoveTemp(Movement));
    }

    if (Packet.bAimActive)
    {
        FBBBCharacterAimPacket Aim;
        Aim.AimTargetWorld = Packet.AimTargetWorld;
        Aim.bAim = Packet.bAim;
        Character->SubmitInput(MoveTemp(Aim));
    }

    if (Packet.bFire)
    {
        Character->SubmitInput(FBBBFirePacket{});
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitActionPacket_Implementation(
    FBBBCharacterActionNetworkPacket Packet)
{
    if (!ensureMsgf(Character && IsOwnerAuthority(),
        TEXT("[UBBBC]Action command arrived without an authoritative character")))
    {
        return;
    }

    if (Packet.bEquipSlot)
    {
        FBBBEquipSlotPacket Equip;
        Equip.Slot = Packet.EquipSlot;
        Character->SubmitInput(MoveTemp(Equip));
    }

    if (Packet.bReload)
    {
        Character->SubmitInput(FBBBReloadPacket{});
    }
}

void UBBBCharacterNetworkComponent::MulticastEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    if (!Character || IsOwnerAuthority())
    {
        return;
    }

    FBBBRestoreEquipmentPacket Restore;
    Restore.EquipmentHandle = Packet.EquipmentHandle;
    Character->SubmitInput(Restore);
}

void UBBBCharacterNetworkComponent::MulticastEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!Character || IsOwnerAuthority())
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
