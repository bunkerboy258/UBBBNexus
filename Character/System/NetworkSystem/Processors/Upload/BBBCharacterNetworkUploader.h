
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBAimUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBFireUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBEquipmentUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBReloadUploadProcessor.h"
class UBBBCharacterNetworkComponent;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemActionResults;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBCharacterNetworkUploader final
{
public:

    /**
     * 逐帧将本地状态变化生成网络Packet并上传
     * @param NetworkData	网络运行时数据
     * @param WorldTimeSeconds	当前世界时间
     * @param AimData	瞄准运行时数据
     * @param AimConfig	瞄准配置
     * @param EquipmentState	角色装备状态
     * @param ActionResults	装备动作结果
     * @param NetworkComponent	角色网络组件
     */
    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        float WorldTimeSeconds,
        const FBBBAimRuntimeData &AimData,
        const FBBBAimConfig &AimConfig,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterItemActionResults &ActionResults,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
private:

    FBBBEquipmentUploadProcessor EquipmentUploadProcessor;
    FBBBFireUploadProcessor FireUploadProcessor;
    FBBBReloadUploadProcessor ReloadUploadProcessor;
    FBBBAimUploadProcessor AimUploadProcessor;
};
