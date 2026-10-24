#pragma once

#include "BBBMonsterGibPartPresentation.generated.h"

/** 少量预制残肢的飞行配置 只在断开时查询 */
USTRUCT(BlueprintType)
struct FBBBMonsterGibPartPresentation final
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (DisplayName = "断开骨骼"))
    FName Bone;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "前向速度", ClampMin = "0", ClampMax = "2000", Units = "cm/s"))
    float Speed = 260;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "上抛速度", ClampMin = "0", ClampMax = "1000", Units = "cm/s"))
    float Lift = 100;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "旋转速度", ClampMin = "0", ClampMax = "1500", Units = "deg/s"))
    float Spin = 180;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "质量", ClampMin = "0.1", ClampMax = "30", Units = "kg"))
    float Mass = 11;
};
