
#pragma once
#include "CoreMinimal.h"
struct FBBBInputRawData;
struct FBBBInputRuntimeData;
struct FBBBInputPipelineConfig;
struct FBBBRawInputFrame;

class ABBB_EVAC_API FBBBInputProcessor final
{
public:

    void Update(
        const FBBBInputRawData &InputRawData,
        FBBBInputRuntimeData &InputData,
        const FBBBInputPipelineConfig &Config,
        float DeltaSeconds) const;
};
