
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBAimUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBFireUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBReloadUploadProcessor.h"
class UBBBCharacterNetworkComponent;
struct FBBBCharacterNetworkConfig;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterEquipmentResults;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBCharacterNetworkUploader final
{
public:

    /**
     * 逐帧将本地状态变化生成网络Packet并上传
     * @param NetworkData	网络运行时数据
     * @param WorldTimeSeconds	当前世界时间
     * @param AimData	瞄准运行时数据
     * @param NetworkConfig	网络配置
     * @param EquipmentState	角色装备状态
     * @param EquipmentResults	装备动作结果
     * @param NetworkComponent	角色网络组件
     */
    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        float WorldTimeSeconds,
        const FBBBAimRuntimeData &AimData,
        const FBBBCharacterNetworkConfig &NetworkConfig,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterEquipmentResults &EquipmentResults,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
private:

    FBBBEquipmentUploadProcessor EquipmentUploadProcessor;
    FBBBFireUploadProcessor FireUploadProcessor;
    FBBBReloadUploadProcessor ReloadUploadProcessor;
    FBBBAimUploadProcessor AimUploadProcessor;
};
