#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/BBBCharacterNetworkRestorer.h"

#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

void FBBBCharacterNetworkRestorer::Update(
    FBBBNetworkRuntimeData &NetworkData,
    FBBBAimRuntimeData &AimData,
    FBBBCharacterLocomotionRuntimeData &LocomotionData,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    UBBBEquipmentCatalog &EquipmentCatalog) const
{
    TArray<FBBBEquipmentNetworkPacket> PendingEquipmentPackets = NetworkData.RestoreEquipmentPackets();
    for (const FBBBEquipmentNetworkPacket &Packet : PendingEquipmentPackets)
    {
        EquipmentRestoreProcessor.Update(EquipmentCommands, Packet, EquipmentCatalog);
    }

    TArray<FBBBEquipmentActionNetworkPacket> PendingActionPackets = NetworkData.RestoreEquipmentActionPackets();
    for (const FBBBEquipmentActionNetworkPacket &Packet : PendingActionPackets)
    {
        EquipmentActionRestoreProcessor.Update(EquipmentCommands, Packet);
    }

    const TOptional<FBBBAimNetworkState> PendingAimState = NetworkData.RestoreAimState();
    if (PendingAimState.IsSet())
    {
        AimRestoreProcessor.Update(AimData, PendingAimState.GetValue());
    }

    const TOptional<FBBBLocomotionNetworkState> PendingLocomotionState = NetworkData.RestoreLocomotionState();
    if (PendingLocomotionState.IsSet())
    {
        LocomotionData.CommitGait(PendingLocomotionState->Gait);
    }
}
