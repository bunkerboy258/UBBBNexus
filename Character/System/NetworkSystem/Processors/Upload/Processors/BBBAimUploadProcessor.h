
#pragma once
#include "CoreMinimal.h"
class UBBBCharacterNetworkComponent;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBAimUploadProcessor final
{
public:

    void Update(
        const FBBBAimRuntimeData &AimData,
        const FBBBAimConfig &AimConfig,
        float WorldTimeSeconds,
        FBBBNetworkRuntimeData &NetworkData,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};
