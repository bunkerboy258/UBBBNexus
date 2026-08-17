
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBAimRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBFireRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBReloadRestoreProcessor.h"
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentCommands;
struct FBBBNetworkRuntimeData;
class UBBBEquipmentCatalog;

class ABBB_EVAC_API FBBBCharacterNetworkRestorer final
{
public:

    /**
     * 逐帧取出并应用所有待还原网络Packet与瞄准状态
     * @param NetworkData	网络运行时数据
     * @param AimData	瞄准运行时数据
     * @param EquipmentCommands	角色物品指令
     * @param EquipmentCatalog	装备目录
     */
    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        FBBBAimRuntimeData &AimData,
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        UBBBEquipmentCatalog &EquipmentCatalog) const;
private:

    //保存装备还原处理器供所属对象后续流程使用
    FBBBEquipmentRestoreProcessor EquipmentRestoreProcessor;

    //保存射击还原处理器供所属对象后续流程使用
    FBBBFireRestoreProcessor FireRestoreProcessor;

    //保存换弹还原处理器供所属对象后续流程使用
    FBBBReloadRestoreProcessor ReloadRestoreProcessor;

    //保存瞄准还原处理器供所属对象后续流程使用
    FBBBAimRestoreProcessor AimRestoreProcessor;
};
