
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/BBBCharacterNetworkUploader.h"

void FBBBCharacterNetworkUploader::Update(
    FBBBNetworkRuntimeData &NetworkData,
    float WorldTimeSeconds,
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterNetworkConfig &NetworkConfig,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterEquipmentResults &EquipmentResults,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{

    EquipmentUploadProcessor.Update(EquipmentState, NetworkData, NetworkSystem);
    FireUploadProcessor.Update(EquipmentResults, NetworkSystem);
    ReloadUploadProcessor.Update(EquipmentResults, NetworkSystem);
    AimUploadProcessor.Update(AimData, NetworkConfig, WorldTimeSeconds, NetworkData, NetworkSystem);
}
