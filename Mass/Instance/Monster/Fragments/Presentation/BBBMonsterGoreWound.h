#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterGoreWound.generated.h"

/** 当前一处投影伤口 断肢与局部创口使用相同数据 不保存历史 */
USTRUCT()
struct FBBBMonsterGoreWound final
{
    GENERATED_BODY()

    /** 零为空 其余为预设稳定编号 */
    UPROPERTY()
    uint8 PresetId = 0;

    /** 当前血层与裁剪权重 */
    UPROPERTY()
    float Strength = 0;

    /** 普通表面伤位置 永久伤口使用预设位置 */
    UPROPERTY()
    FVector Center = FVector::ZeroVector;

    /** 参考姿态中的投影方向 */
    UPROPERTY()
    FQuat Orientation = FQuat::Identity;

    /** 普通伤口半径和半深 */
    UPROPERTY()
    FVector2D RadiusDepth = FVector2D(1.8, 1.8);
};
