#pragma once

#include "CoreMinimal.h"
#include "BBBFacingStates.generated.h"

/** 角色当前朝向状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterFacingState
{
    GENERATED_BODY()

    /** 身体是否正在追赶瞄准方向 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsBodyTurning = false;

    /** 当前水平瞄准偏角绝对值 */
    UPROPERTY(BlueprintReadOnly)
    float CurrentAimYawAbs = 0.0f;
};
