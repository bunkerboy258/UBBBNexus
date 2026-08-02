
#pragma once
#include "CoreMinimal.h"
struct FBBBAimNetworkState;
struct FBBBAimRuntimeData;

class ABBB_EVAC_API FBBBAimRestoreProcessor final
{
public:

    void Update(
        FBBBAimRuntimeData &AimData,
        const FBBBAimNetworkState &AimState) const;
};
