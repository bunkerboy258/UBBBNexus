#pragma once
#include "CoreMinimal.h"
#include "BBBPlayerCameraInput.generated.h"

/** 外部贡献的相机冲量 */
USTRUCT()
struct FBBBPlayerCameraInput
{
    GENERATED_BODY()

    /** 上下 左右 倾斜的角度冲击 */
    FVector Impulse = FVector::ZeroVector;

};
