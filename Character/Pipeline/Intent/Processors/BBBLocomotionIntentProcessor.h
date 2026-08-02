
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBCharacterLocomotionConfig;
struct FBBBInputRuntimeData;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBLocomotionIntentProcessor final
{
public:

    void Update(
        const FBBBInputRuntimeData &InputData,
        const FBBBIntentRuntimeData &IntentData,
        const FBBBCharacterLocomotionConfig &Config,
        float DeltaSeconds,
        FBBBCharacterIntentState &Intent) const;
};
