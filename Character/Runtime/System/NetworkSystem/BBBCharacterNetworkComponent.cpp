#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
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

    FBBBRestoreEquipmentPacket Restore;
    Restore.EquipmentHandle = Packet.EquipmentHandle;
    Input->Submit(Restore);
}

void UBBBCharacterNetworkComponent::MulticastEquipmentActionPacket_Implementation(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!Input || IsOwnerLocallyControlled())
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
        Input->Submit(Restore);
        break;
    }
    case FBBBReloadStartedFactPacket::PacketId:
    {
        FBBBReloadStartedFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Input->Submit(Restore);
        break;
    }
    case FBBBMagazineDetachedFactPacket::PacketId:
    {
        FBBBMagazineDetachedFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Input->Submit(Restore);
        break;
    }
    case FBBBMagazineLoadedFactPacket::PacketId:
    {
        FBBBMagazineLoadedFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Input->Submit(Restore);
        break;
    }
    case FBBBReloadCancelledFactPacket::PacketId:
    {
        FBBBReloadCancelledFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Input->Submit(Restore);
        break;
    }
    case FBBBEquipFactPacket::PacketId:
    {
        FBBBEquipFactPacket Restore;
        Restore.EquipmentId = Packet.EquipmentId;
        Restore.Sequence = Packet.Sequence;
        Restore.LoadedAmmo = Packet.LoadedAmmo;
        Input->Submit(Restore);
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
    if (Input && !IsOwnerLocallyControlled())
    {
        FBBBRestoreAimPacket Restore;
        Restore.State = FBBBAimRuntimeState{AimState.bIsAiming, AimState.AimTargetWorld};
        Input->Submit(Restore);
    }
}

void UBBBCharacterNetworkComponent::ServerSubmitLocomotionState_Implementation(FBBBLocomotionNetworkState LocomotionState)
{
    SetReplicatedLocomotionState(LocomotionState);
    if (Input && !IsOwnerLocallyControlled())
    {
        FBBBRestoreLocomotionPacket Restore;
        Restore.Gait = LocomotionState.Gait;
        Input->Submit(Restore);
    }
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    if (!Input)
    {
        return;
    }

    FBBBRestoreAimPacket Restore;
    Restore.State = FBBBAimRuntimeState{ReplicatedAimState.bIsAiming, ReplicatedAimState.AimTargetWorld};
    Input->Submit(Restore);
}

void UBBBCharacterNetworkComponent::OnRep_ReplicatedLocomotionState()
{
    if (!Input)
    {
        return;
    }

    FBBBRestoreLocomotionPacket Restore;
    Restore.Gait = ReplicatedLocomotionState.Gait;
    Input->Submit(Restore);
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
