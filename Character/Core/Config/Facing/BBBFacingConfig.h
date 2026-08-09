#pragma once
#include "CoreMinimal.h"
#include "BBBFacingConfig.generated.h"

USTRUCT(BlueprintType)
//定义武装状态下身体转向参数
struct FBBBCharacterFacingConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxAimYawBeforeBodyTurn = 40.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimYawBodyTurnStopThreshold = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ArmedBodyTurnInterpSpeed = 8.0f;
};
