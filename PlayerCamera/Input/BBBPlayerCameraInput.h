#pragma once
#include "CoreMinimal.h"
#include "BBBPlayerCameraInput.generated.h"
struct FBBBCharacterRuntimeData;

/** 外部贡献的相机冲量 */
USTRUCT()
struct FBBBPlayerCameraInput
{
    GENERATED_BODY()

    FVector2D Impulse = FVector2D::ZeroVector;
    float RecoverySpeed = 1.0f;

    /** @return 相机冲量是否有效 */
    bool CanApply(const FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    void Apply(FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    static void BeginFrame(FBBBCharacterRuntimeData &Data);
};
