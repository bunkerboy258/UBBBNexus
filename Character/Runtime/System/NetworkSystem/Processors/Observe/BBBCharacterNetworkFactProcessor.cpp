#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/Observe/BBBCharacterNetworkFactProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"

void FBBBCharacterNetworkFactProcessor::Update(
    FBBBNetworkState &NetworkData,
    const float WorldTimeSeconds,
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterLocomotionRuntimeData &LocomotionData,
    const FBBBCharacterNetworkConfig &NetworkConfig,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    // 按装备动作瞄准和移动顺序上传本帧网络状态
    EquipmentUploadProcessor.Update(EquipmentState, NetworkData, NetworkSystem);
    EquipmentActionUploadProcessor.Update(EquipmentEvents, NetworkSystem);
    AimUploadProcessor.Update(AimData, NetworkConfig, WorldTimeSeconds, NetworkData, NetworkSystem);
    LocomotionUploadProcessor.Update(LocomotionData, NetworkData, NetworkSystem);
}
