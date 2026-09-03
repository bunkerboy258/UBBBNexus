#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/BBBCharacterNetworkUploader.h"

#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"

void FBBBCharacterNetworkUploader::Update(
    FBBBNetworkRuntimeData &NetworkData,
    const float WorldTimeSeconds,
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterLocomotionRuntimeData &LocomotionData,
    const FBBBCharacterNetworkConfig &NetworkConfig,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    EquipmentUploadProcessor.Update(EquipmentState, NetworkData, NetworkSystem);
    EquipmentActionUploadProcessor.Update(EquipmentEvents, NetworkSystem);
    AimUploadProcessor.Update(AimData, NetworkConfig, WorldTimeSeconds, NetworkData, NetworkSystem);
    LocomotionUploadProcessor.Update(LocomotionData, NetworkData, NetworkSystem);
}
