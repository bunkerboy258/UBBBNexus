#pragma once

#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBAimUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentActionUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBLocomotionUploadProcessor.h"

class FBBBCharacterNetworkSystem;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterLocomotionRuntimeData;
struct FBBBCharacterNetworkConfig;
struct FBBBNetworkRuntimeData;

/** 上传角色本地状态与动作事件 */
class ABBB_EVAC_API FBBBCharacterNetworkUploader final
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
        FBBBNetworkRuntimeData &NetworkData,
        float WorldTimeSeconds,
        const FBBBAimRuntimeData &AimData,
        const FBBBCharacterLocomotionRuntimeData &LocomotionData,
        const FBBBCharacterNetworkConfig &NetworkConfig,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterEquipmentEvents &EquipmentEvents,
        FBBBCharacterNetworkSystem &NetworkSystem) const;

private:
    FBBBEquipmentUploadProcessor EquipmentUploadProcessor;

    FBBBEquipmentActionUploadProcessor EquipmentActionUploadProcessor;

    FBBBAimUploadProcessor AimUploadProcessor;

    FBBBLocomotionUploadProcessor LocomotionUploadProcessor;
};
