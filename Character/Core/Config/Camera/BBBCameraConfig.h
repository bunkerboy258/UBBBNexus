#pragma once
#include "CoreMinimal.h"
#include "BBBCameraConfig.generated.h"

USTRUCT(BlueprintType)
//定义第三人称相机基础与瞄准参数
struct FBBBCharacterCameraConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseTurnRate = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float CameraBoomLength = 400.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector CameraBoomSocketOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector CameraBoomTargetOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector CameraRelativeLocation = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bCameraLag = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float CameraLagSpeed = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimBoomLength = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimBoomInterpSpeed = 12.0f;
};
