#pragma once
#include "CoreMinimal.h"
#include "BBBCameraStates.generated.h"

USTRUCT(BlueprintType)
//保存相机系统当前状态
struct FBBBCameraState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FVector2D AppliedRecoilOffset = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    float RecoilRecoverySpeed = 8.0f;
};
