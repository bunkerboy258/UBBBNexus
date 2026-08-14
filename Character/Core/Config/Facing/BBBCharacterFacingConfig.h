#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterFacingConfig.generated.h"

/**
 * 配置角色身体朝向的原地滞回区间与旋转速度
 */
USTRUCT(BlueprintType)
struct FBBBCharacterFacingConfig
{
    GENERATED_BODY()

    /** 静止角色开始追赶相机方向的最小角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Facing", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float IdleTurnStartAngle = 70.0f;

    /** 静止角色停止追赶相机方向的最大角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Facing", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float IdleTurnStopAngle = 10.0f;

    /** 静止角色原地转向的最大水平旋转速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Facing", meta = (ClampMin = "0.0"))
    float IdleTurnRotationRate = 360.0f;

    /** 角色存在移动输入时追赶相机方向的最大水平旋转速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Facing", meta = (ClampMin = "0.0"))
    float MoveAlignmentRotationRate = 720.0f;
};
