
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
struct FBBBCharacterItemEvents;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBCharacterNetworkUploader final
{
public:

    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        float WorldTimeSeconds,
        const FBBBAimRuntimeData &AimData,
        const FBBBAimConfig &AimConfig,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterItemEvents &ItemEvents,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
private:

    FBBBEquipmentUploadProcessor EquipmentUploadProcessor;
    FBBBFireUploadProcessor FireUploadProcessor;
    FBBBReloadUploadProcessor ReloadUploadProcessor;
    FBBBAimUploadProcessor AimUploadProcessor;
};
