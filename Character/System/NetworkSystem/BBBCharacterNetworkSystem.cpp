#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkRuntimeData &InNetworkData,
    FBBBAimRuntimeData &InAimData,
    FBBBCharacterLocomotionRuntimeData &InLocomotionData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    UBBBEquipmentCatalog &InEquipmentCatalog,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    FBBBCharacterEquipmentCommands &InEquipmentCommands,
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterNetworkConfig &InNetworkConfig)
{
    NetworkData = &InNetworkData;
    AimData = &InAimData;
    LocomotionData = &InLocomotionData;
    EquipmentState = &InEquipmentState;
    NetworkComponent = &InNetworkComponent;
    EquipmentCatalog = &InEquipmentCatalog;
    WorldData = &InWorldData;
    EquipmentCommands = &InEquipmentCommands;
    EquipmentEvents = &InEquipmentEvents;
    NetworkConfig = &InNetworkConfig;
}

void FBBBCharacterNetworkSystem::UpdateRestore()
{
    if (!ensureMsgf(NetworkData && AimData && LocomotionData && EquipmentCommands && EquipmentCatalog, TEXT("[UBBBC]Network restore dependencies are null")))
    {
        return;
    }

    Restorer.Update(
        *NetworkData,
        *AimData,
        *LocomotionData,
        *EquipmentCommands,
        *EquipmentCatalog);
}

void FBBBCharacterNetworkSystem::UpdateUpload()
{
    if (!ensureMsgf(NetworkData && WorldData && AimData && LocomotionData && NetworkConfig && EquipmentState && EquipmentEvents && NetworkComponent, TEXT("[UBBBC]Network upload dependencies are null")))
    {
        return;
    }

    Uploader.Update(
        *NetworkData,
        WorldData->GetWorldTimeSeconds(),
        *AimData,
        *LocomotionData,
        *NetworkConfig,
        *EquipmentState,
        *EquipmentEvents,
        *this);
}

void FBBBCharacterNetworkSystem::SubmitEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Equipment packet cannot be submitted")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        ReceiveEquipmentForDistribution(MoveTemp(Packet));
        return;
    }

    NetworkComponent->ServerUploadEquipmentPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Equipment action packet cannot be submitted")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        ReceiveEquipmentActionForDistribution(MoveTemp(Packet));
        return;
    }

    NetworkComponent->ServerUploadEquipmentActionPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveEquipmentForDistribution(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Equipment packet cannot be distributed")))
    {
        return;
    }

    NetworkComponent->MulticastEquipmentPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveEquipmentForRestore(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkData && NetworkComponent, TEXT("[UBBBC]Equipment restore packet cannot be queued")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        return;
    }

    NetworkData->EnqueueRestoreEquipmentPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveEquipmentActionForDistribution(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Equipment action packet cannot be distributed")))
    {
        return;
    }

    NetworkComponent->MulticastEquipmentActionPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveEquipmentActionForRestore(FBBBEquipmentActionNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkData && NetworkComponent, TEXT("[UBBBC]Equipment action restore packet cannot be queued")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        return;
    }

    NetworkData->EnqueueRestoreEquipmentActionPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Aim state cannot be submitted")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        ReceiveSubmittedAimState(AimState);
        return;
    }

    NetworkComponent->ServerSubmitAimState(AimState);
}

void FBBBCharacterNetworkSystem::SubmitLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Locomotion state cannot be submitted")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        ReceiveSubmittedLocomotionState(LocomotionState);
        return;
    }

    NetworkComponent->ServerSubmitLocomotionState(LocomotionState);
}

void FBBBCharacterNetworkSystem::ReceiveSubmittedAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(NetworkData && NetworkComponent, TEXT("[UBBBC]Submitted aim state cannot be processed")))
    {
        return;
    }

    NetworkComponent->SetReplicatedAimState(AimState);
    if (!NetworkComponent->IsOwnerLocallyControlled())
    {
        NetworkData->SetPendingRestoreAimState(AimState);
    }
}

void FBBBCharacterNetworkSystem::ReceiveReplicatedAimState(const FBBBAimNetworkState &AimState)
{
    if (ensureMsgf(NetworkData, TEXT("[UBBBC]Replicated aim state cannot be queued")))
    {
        NetworkData->SetPendingRestoreAimState(AimState);
    }
}

void FBBBCharacterNetworkSystem::ReceiveSubmittedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (!ensureMsgf(NetworkData && NetworkComponent, TEXT("[UBBBC]Submitted locomotion state cannot be processed")))
    {
        return;
    }

    NetworkComponent->SetReplicatedLocomotionState(LocomotionState);
    if (!NetworkComponent->IsOwnerLocallyControlled())
    {
        NetworkData->SetPendingRestoreLocomotionState(LocomotionState);
    }
}

void FBBBCharacterNetworkSystem::ReceiveReplicatedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState)
{
    if (ensureMsgf(NetworkData, TEXT("[UBBBC]Replicated locomotion state cannot be queued")))
    {
        NetworkData->SetPendingRestoreLocomotionState(LocomotionState);
    }
}
