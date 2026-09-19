#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/States/BBBCharacterAimState.h"
#include "BBBWork/UBBBNexus/Character/Input/States/BBBCharacterMovementState.h"
#include "BBBCharacterInputStates.generated.h"

/** 外部控制源提交的全部持续状态 覆盖式更新并跨帧保留 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBCharacterInputStates
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterMovementState Movement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBBBCharacterAimState Aim;
};
