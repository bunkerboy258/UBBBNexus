#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBCharacterNetworkObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"

void FBBBCharacterNetworkObservationProcessor::Update(
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
    EquipmentStateObservationProcessor.Update(EquipmentState, NetworkData, NetworkSystem);
    EquipmentFactObservationProcessor.Update(EquipmentEvents, NetworkSystem);
    AimObservationProcessor.Update(AimData, NetworkConfig, WorldTimeSeconds, NetworkData, NetworkSystem);
    LocomotionObservationProcessor.Update(LocomotionData, NetworkData, NetworkSystem);
}
