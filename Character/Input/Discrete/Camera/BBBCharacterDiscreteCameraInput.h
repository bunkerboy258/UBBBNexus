#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterDiscreteCameraInput.generated.h"

/** 相机表现贡献输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteCameraInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Impulse = FVector2D::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RecoverySpeed = 0.0f;
};
