#pragma once

#include "CoreMinimal.h"
#include "BBBSMGHandlingSettings.generated.h"

/** 冲锋枪在一种持枪姿态下提供的表现参数 */
USTRUCT(BlueprintType)
struct FBBBSMGHandlingSettings
{
    GENERATED_BODY()

    /** 枪口追随目标的响应速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "目标跟随", meta = (DisplayPriority = "1", ClampMin = "0.01", DisplayName = "目标跟随速率 (1/秒)", ToolTip = "指数跟随系数 越大枪口追随目标越快 不是度/秒 不控制后坐力回零"))
    float AimFollowSpeed = 18.0f;

    /** 当前姿态对基础轨迹的幅度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "后坐力轨迹", meta = (ClampMin = "0.0", DisplayName = "轨迹幅度倍率", ToolTip = "无单位 1 使用基础轨迹 0 无方向后坐力 越大姿态偏移越大 不改变镜头"))
    float RecoilScale = 1.0f;

    /** 向后受力动画的混合强度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "后向震动", meta = (DisplayPriority = "5", ClampMin = "0.0", ClampMax = "1.0", DisplayName = "后向震动权重", ToolTip = "无单位 0 无震动 1 为专属动画完整幅度 越大震动越强 每枪重播 换弹时关闭"))
    float BackwardRecoilAlpha = 0.45f;

    /** 连续持枪摇摆的上下与左右幅度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "持枪摇摆", meta = (DisplayPriority = "6", DisplayName = "摇摆幅度 (度)", ToolTip = "X 上下 Y 左右 正弦摇摆的峰值 绝对值越大摇摆越大 0 无摇摆 影响实际枪口方向"))
    FVector2D SwayAmplitudeDegrees = FVector2D(0.25f, 0.35f);

    /** 连续持枪摇摆的上下与左右频率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "持枪摇摆", meta = (DisplayPriority = "7", DisplayName = "摇摆频率 (赫兹)", ToolTip = "X 上下 Y 左右 每秒完整往复次数 越大摇摆越快 0 使该轴摇摆为零"))
    FVector2D SwayFrequency = FVector2D(0.65f, 0.43f);
};
