#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"

void FBBBCharacterNetworkSystem::Initialize(
    FBBBNetworkRuntimeData &InNetworkData,
    FBBBAimRuntimeData &InAimData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    UBBBCharacterNetworkComponent &InNetworkComponent,
    UBBBEquipmentCatalog &InEquipmentCatalog,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    FBBBCharacterEquipmentCommands &InEquipmentCommands,
    const FBBBCharacterEquipmentResults &InEquipmentResults,
    const FBBBCharacterNetworkConfig &InNetworkConfig)
{
    NetworkData = &InNetworkData;
    AimData = &InAimData;
    EquipmentState = &InEquipmentState;
    NetworkComponent = &InNetworkComponent;
    EquipmentCatalog = &InEquipmentCatalog;
    WorldData = &InWorldData;
    EquipmentCommands = &InEquipmentCommands;
    EquipmentResults = &InEquipmentResults;
    NetworkConfig = &InNetworkConfig;
}

void FBBBCharacterNetworkSystem::UpdateValidation()
{
    if (!ensureMsgf(NetworkData && NetworkComponent && EquipmentCatalog, TEXT("[UBBBC]Network validation dependencies are null")))
    {
        return;
    }

    TArray<FBBBEquipmentNetworkPacket> ValidPackets = Validator.Update(*NetworkData, *EquipmentCatalog);
    for (FBBBEquipmentNetworkPacket &Packet : ValidPackets)
    {
        NetworkComponent->MulticastEquipmentPacket(MoveTemp(Packet));
    }
}

void FBBBCharacterNetworkSystem::UpdateRestore()
{
    if (!ensureMsgf(NetworkData && AimData && EquipmentCommands && EquipmentCatalog, TEXT("[UBBBC]Network restore dependencies are null")))
    {
        return;
    }

    Restorer.Update(
        *NetworkData,
        *AimData,
        *EquipmentCommands,
        *EquipmentCatalog);
}

void FBBBCharacterNetworkSystem::UpdateUpload()
{
    if (!ensureMsgf(NetworkData && WorldData && AimData && NetworkConfig && EquipmentState && EquipmentResults && NetworkComponent, TEXT("[UBBBC]Network upload dependencies are null")))
    {
        return;
    }

    Uploader.Update(
        *NetworkData,
        WorldData->GetWorldTimeSeconds(),
        *AimData,
        *NetworkConfig,
        *EquipmentState,
        *EquipmentResults,
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
        ReceiveEquipmentForValidation(MoveTemp(Packet));
        return;
    }

    NetworkComponent->ServerUploadEquipmentPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitFirePacket(FBBBFireNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Fire packet cannot be submitted")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        ReceiveFireForDistribution(MoveTemp(Packet));
        return;
    }

    NetworkComponent->ServerUploadFirePacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::SubmitReloadPacket(FBBBReloadNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Reload packet cannot be submitted")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerAuthority())
    {
        ReceiveReloadForDistribution(MoveTemp(Packet));
        return;
    }

    NetworkComponent->ServerUploadReloadPacket(MoveTemp(Packet));
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

void FBBBCharacterNetworkSystem::ReceiveEquipmentForValidation(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Equipment validation packet cannot be queued")))
    {
        return;
    }

    NetworkData->EnqueueValidationEquipmentPacket(MoveTemp(Packet));
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

void FBBBCharacterNetworkSystem::ReceiveFireForDistribution(FBBBFireNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Fire packet cannot be distributed")))
    {
        return;
    }

    NetworkComponent->MulticastFirePacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveFireForRestore(FBBBFireNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkData && NetworkComponent, TEXT("[UBBBC]Fire restore packet cannot be queued")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        return;
    }

    NetworkData->EnqueueRestoreFirePacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveReloadForDistribution(FBBBReloadNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkComponent, TEXT("[UBBBC]Reload packet cannot be distributed")))
    {
        return;
    }

    NetworkComponent->MulticastReloadPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveReloadForRestore(FBBBReloadNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkData && NetworkComponent, TEXT("[UBBBC]Reload restore packet cannot be queued")))
    {
        return;
    }

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        return;
    }

    NetworkData->EnqueueRestoreReloadPacket(MoveTemp(Packet));
}

void FBBBCharacterNetworkSystem::ReceiveSubmittedAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(NetworkData && NetworkComponent, TEXT("[UBBBC]Submitted aim state cannot be processed")))
    {
        return;
    }

    NetworkComponent->SetReplicatedAimState(AimState);

    if (NetworkComponent->IsOwnerLocallyControlled())
    {
        return;
    }

    NetworkData->SetPendingRestoreAimState(AimState);
}

void FBBBCharacterNetworkSystem::ReceiveReplicatedAimState(const FBBBAimNetworkState &AimState)
{
    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Replicated aim state cannot be queued")))
    {
        return;
    }

    NetworkData->SetPendingRestoreAimState(AimState);
}
