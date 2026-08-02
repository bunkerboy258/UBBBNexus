
#pragma once
#include "CoreMinimal.h"
#include "BBBFacingTypes.generated.h"

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

USTRUCT(BlueprintType)
//定义武装状态下身体转向阈值与速度
struct FBBBCharacterFacingConfig
{
    GENERATED_BODY()

    //超过该水平偏角后开始旋转身体
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxAimYawBeforeBodyTurn = 40.0f;

    //偏角回落到该值后停止身体追赶
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimYawBodyTurnStopThreshold = 10.0f;

    //控制武装身体朝目标方向旋转的速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ArmedBodyTurnInterpSpeed = 8.0f;
};
