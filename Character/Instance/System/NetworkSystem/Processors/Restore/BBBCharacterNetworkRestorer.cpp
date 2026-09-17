#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Restore/BBBCharacterNetworkRestorer.h"

#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

void FBBBCharacterNetworkRestorer::Update(
    FBBBNetworkRuntimeData &NetworkData,
    FBBBAimRuntimeData &AimData,
    FBBBCharacterLocomotionRuntimeData &LocomotionData,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    UBBBEquipmentCatalog &EquipmentCatalog) const
{
    // 先取出装备状态包并恢复装备目标
    TArray<FBBBEquipmentNetworkPacket> PendingEquipmentPackets = NetworkData.RestoreEquipmentPackets();
    for (const FBBBEquipmentNetworkPacket &Packet : PendingEquipmentPackets)
    {
        EquipmentRestoreProcessor.Update(EquipmentCommands, Packet, EquipmentCatalog);
    }

    // 再取出装备动作包并恢复装备执行事实
    TArray<FBBBEquipmentActionNetworkPacket> PendingActionPackets = NetworkData.RestoreEquipmentActionPackets();
    for (const FBBBEquipmentActionNetworkPacket &Packet : PendingActionPackets)
    {
        EquipmentActionRestoreProcessor.Update(EquipmentCommands, Packet);
    }

    // 有新的瞄准状态时恢复远端瞄准数据
    const TOptional<FBBBAimNetworkState> PendingAimState = NetworkData.RestoreAimState();
    if (PendingAimState.IsSet())
    {
        AimRestoreProcessor.Update(AimData, PendingAimState.GetValue());
    }

    // 有新的移动状态时恢复远端步态数据
    const TOptional<FBBBLocomotionNetworkState> PendingLocomotionState = NetworkData.RestoreLocomotionState();
    if (PendingLocomotionState.IsSet())
    {
        LocomotionData.CommitGait(PendingLocomotionState->Gait);
    }
}
