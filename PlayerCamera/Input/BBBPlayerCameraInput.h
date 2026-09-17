#pragma once
#include "CoreMinimal.h"
#include "BBBPlayerCameraInput.generated.h"

/** 外部贡献的相机冲量 */
USTRUCT()
struct FBBBPlayerCameraInput
{
    GENERATED_BODY()

    FVector2D Impulse = FVector2D::ZeroVector;
    float RecoverySpeed = 1.0f;
};
