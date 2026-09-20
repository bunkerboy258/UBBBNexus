#pragma once

#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBAimObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBEquipmentFactObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBEquipmentStateObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBLocomotionObservationProcessor.h"

class FBBBCharacterNetworkSystem;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBCharacterNetworkConfig;
struct FBBBNetworkState;

/** 上传角色本地状态与动作事件 */
class ABBB_EVAC_API FBBBCharacterNetworkObservationProcessor final
{
public:
    /**
     * 上传本帧变化
     * @param NetworkData 网络运行时数据
     * @param WorldTimeSeconds 当前世界时间
     * @param AimData 瞄准运行时数据
     * @param LocomotionData 移动运行时数据
     * @param NetworkConfig 网络配置
     * @param EquipmentState 装备状态
     * @param EquipmentEvents 装备事件
     * @param NetworkSystem 网络系统
     */
    void Update(
        FBBBNetworkState &NetworkData,
        float WorldTimeSeconds,
        const FBBBAimRuntimeData &AimData,
        const FBBBCharacterLocomotionRuntimeData &LocomotionData,
        const FBBBCharacterNetworkConfig &NetworkConfig,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterEquipmentEvents &EquipmentEvents,
        FBBBCharacterNetworkSystem &NetworkSystem) const;

private:
    FBBBEquipmentStateObservationProcessor EquipmentStateObservationProcessor;

    FBBBEquipmentFactObservationProcessor EquipmentFactObservationProcessor;

    FBBBAimObservationProcessor AimObservationProcessor;

    FBBBLocomotionObservationProcessor LocomotionObservationProcessor;
};
