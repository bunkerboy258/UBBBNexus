#pragma once

#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBAimRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentActionRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"

class UBBBEquipmentCatalog;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentCommands;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBNetworkRuntimeData;

/** 还原远端角色状态与动作 */
class ABBB_EVAC_API FBBBCharacterNetworkRestorer final
{
public:
    /**
     * 还原本帧远端数据
     * @param NetworkData 网络运行时数据
     * @param AimData 瞄准运行时数据
     * @param LocomotionData 移动运行时数据
     * @param EquipmentCommands 装备命令
     * @param EquipmentCatalog 装备目录
     */
    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        FBBBAimRuntimeData &AimData,
        FBBBCharacterLocomotionRuntimeData &LocomotionData,
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        UBBBEquipmentCatalog &EquipmentCatalog) const;

private:
    FBBBEquipmentRestoreProcessor EquipmentRestoreProcessor;

    FBBBEquipmentActionRestoreProcessor EquipmentActionRestoreProcessor;

    FBBBAimRestoreProcessor AimRestoreProcessor;
};
