
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBInputRuntimeData;

class ABBB_EVAC_API FBBBInputIntentProcessor final
{
public:

    void Update(const FBBBInputRuntimeData &InputData, FBBBCharacterIntentState &Intent) const;
};
