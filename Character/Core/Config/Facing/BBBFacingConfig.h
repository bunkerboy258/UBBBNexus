#pragma once
#include "CoreMinimal.h"
#include "BBBFacingConfig.generated.h"

USTRUCT(BlueprintType)
//定义武装状态下身体转向参数
struct FBBBCharacterFacingConfig
{
    GENERATED_BODY()

    /** 计算身体相对瞄准方向时使用的角色骨骼 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AimOriginBoneName = FName("spine_03");

    /** 水平瞄准偏角的插值速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimYawInterpSpeed = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxAimYawBeforeBodyTurn = 40.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimYawBodyTurnStopThreshold = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ArmedBodyTurnInterpSpeed = 8.0f;
};
