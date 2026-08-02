
#pragma once
#include "CoreMinimal.h"
#include "BBBFacingRuntimeData.generated.h"

USTRUCT(BlueprintType)
struct FBBBFacingRuntimeData
{
    GENERATED_BODY()

    bool IsBodyTurning() const
    {
        return bIsBodyTurning;
    }

    float GetCurrentAimYawAbs() const
    {
        return CurrentAimYawAbs;
    }

    bool GetLastFaceAimDirection() const
    {
        return bLastFaceAimDirection;
    }

    bool HasAppliedRotationMode() const
    {
        return bHasAppliedRotationMode;
    }

    void CommitState(
        bool bInIsBodyTurning,
        float InCurrentAimYawAbs,
        bool bInLastFaceAimDirection,
        bool bInHasAppliedRotationMode)
    {
        bIsBodyTurning = bInIsBodyTurning;
        CurrentAimYawAbs = InCurrentAimYawAbs;
        bLastFaceAimDirection = bInLastFaceAimDirection;
        bHasAppliedRotationMode = bInHasAppliedRotationMode;
    }
private:

    UPROPERTY()
    bool bIsBodyTurning = false;

    UPROPERTY()
    float CurrentAimYawAbs = 0.0f;

    bool bLastFaceAimDirection = false;

    bool bHasAppliedRotationMode = false;
};
