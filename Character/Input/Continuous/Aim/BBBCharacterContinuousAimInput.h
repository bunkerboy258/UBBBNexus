#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterContinuousAimInput.generated.h"

/** 瞄准持续输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterContinuousAimInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector AimTargetWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAim = false;
};
