
#pragma once
#include "CoreMinimal.h"
#include "BBBFacingStates.generated.h"

USTRUCT(BlueprintType)
//保存身体转向状态与当前水平瞄准偏角
struct FBBBCharacterFacingState
{
    GENERATED_BODY()

    //标记角色身体是否正在追赶视线方向
    UPROPERTY(BlueprintReadOnly)
    bool bIsBodyTurning = false;

    //保存不带符号的当前水平瞄准偏角
    UPROPERTY(BlueprintReadOnly)
    float CurrentAimYawAbs = 0.0f;
};

