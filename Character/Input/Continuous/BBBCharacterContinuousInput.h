#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Continuous/Aim/BBBCharacterContinuousAimInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Continuous/Movement/BBBCharacterContinuousMovementInput.h"
#include "BBBCharacterContinuousInput.generated.h"

/** 外部控制源提交的持续状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterContinuousInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterContinuousMovementInput Movement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterContinuousAimInput Aim;
};
