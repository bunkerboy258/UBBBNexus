#pragma once

#include "CoreMinimal.h"
#include "BBBFacingStates.generated.h"

/** 角色当前朝向状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterFacingState
{
    GENERATED_BODY()

    /** 身体是否正在追赶瞄准方向 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsBodyTurning = false;

    /** 相对角色前轴的水平瞄准偏角 */
    UPROPERTY(BlueprintReadOnly)
    float AimYaw = 0.0f;

    /** 身体朝向计算使用的瞄准起点世界坐标 */
    UPROPERTY(BlueprintReadOnly)
    FVector AimOriginWorld = FVector::ZeroVector;
};
