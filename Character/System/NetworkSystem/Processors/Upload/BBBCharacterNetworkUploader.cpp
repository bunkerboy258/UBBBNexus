
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/BBBCharacterNetworkUploader.h"

void FBBBCharacterNetworkUploader::Update(
    FBBBNetworkRuntimeData &NetworkData,
    float WorldTimeSeconds,
    const FBBBAimRuntimeData &AimData,
    const FBBBAimConfig &AimConfig,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterEquipmentResults &EquipmentResults,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{

    EquipmentUploadProcessor.Update(EquipmentState, NetworkData, NetworkComponent);
    FireUploadProcessor.Update(EquipmentResults, NetworkComponent);
    ReloadUploadProcessor.Update(EquipmentResults, NetworkComponent);
    AimUploadProcessor.Update(AimData, AimConfig, WorldTimeSeconds, NetworkData, NetworkComponent);
}
