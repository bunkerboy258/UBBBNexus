
#pragma once
#include "CoreMinimal.h"
struct FBBBDecisionRuntimeData;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBCharacterActionPlanner final
{
public:

    void Update(
        const FBBBIntentRuntimeData &IntentData,
        FBBBDecisionRuntimeData &DecisionData) const;
};
