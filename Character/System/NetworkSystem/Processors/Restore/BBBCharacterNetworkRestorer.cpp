
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/BBBCharacterNetworkRestorer.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

void FBBBCharacterNetworkRestorer::Update(
    FBBBNetworkRuntimeData &NetworkData,
    FBBBAimRuntimeData &AimData,
    FBBBCharacterEquipmentState &EquipmentState) const
{

    TArray<FBBBEquipmentNetworkPacket> PendingEquipmentPackets = NetworkData.RestoreEquipmentPackets();

    for (const FBBBEquipmentNetworkPacket &Packet : PendingEquipmentPackets)
    {
        EquipmentRestoreProcessor.Update(EquipmentState, Packet);
    }

    TArray<FBBBFireNetworkPacket> PendingFirePackets = NetworkData.RestoreFirePackets();
    for (const FBBBFireNetworkPacket &Packet : PendingFirePackets)
    {
        FireRestoreProcessor.Update(EquipmentState, Packet);
    }

    TArray<FBBBReloadNetworkPacket> PendingReloadPackets = NetworkData.RestoreReloadPackets();
    for (const FBBBReloadNetworkPacket &Packet : PendingReloadPackets)
    {
        ReloadRestoreProcessor.Update(EquipmentState, Packet);
    }

    const TOptional<FBBBAimNetworkState> PendingAimState = NetworkData.RestoreAimState();
    if (PendingAimState.IsSet())
    {
        AimRestoreProcessor.Update(AimData, PendingAimState.GetValue());
    }
}
