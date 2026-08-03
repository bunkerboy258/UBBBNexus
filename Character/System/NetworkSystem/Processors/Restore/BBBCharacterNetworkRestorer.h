
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBAimRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBFireRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBReloadRestoreProcessor.h"
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemCommands;
struct FBBBNetworkRuntimeData;
class UBBBEquipmentCatalog;

class ABBB_EVAC_API FBBBCharacterNetworkRestorer final
{
public:

    /**
     * 逐帧取出并应用所有待还原网络Packet与瞄准状态
     * @param NetworkData	网络运行时数据
     * @param AimData	瞄准运行时数据
     * @param EquipmentState	角色装备状态
     * @param ItemCommands	角色物品指令
     * @param EquipmentCatalog	装备目录
     * @param InstanceOuter	镜像装备实例生命周期所有者
     */
    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        FBBBAimRuntimeData &AimData,
        FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterItemCommands &ItemCommands,
        UBBBEquipmentCatalog &EquipmentCatalog,
        UObject &InstanceOuter) const;
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
