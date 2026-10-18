#pragma once

#include "CoreMinimal.h"
#include "BBBMinigunHandlingSettings.generated.h"

/** 转管机枪在一种持枪姿态下提供的表现参数 */
USTRUCT(BlueprintType)
struct FBBBMinigunHandlingSettings
{
    GENERATED_BODY()

    /** 枪口追随目标的响应速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "目标跟随", meta = (DisplayPriority = "1", ClampMin = "0.01", DisplayName = "目标跟随速率 (1/秒)", ToolTip = "指数跟随系数 越大枪口追随目标越快 不是度/秒 不控制后坐力回零"))
    float AimFollowSpeed = 18.0f;

    /** 每枪向上和向右的角度冲击 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "枪口后坐力", meta = (DisplayPriority = "2", DisplayName = "每枪方向冲击 (度)", ToolTip = "X 向上 Y 向右 负值反向 绝对值越大每枪方向偏移越大 与镜头冲击独立"))
    FVector2D AimImpulseDegrees = FVector2D(2.0f, 0.15f);

    /** 每枪方向冲击的对称随机范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "枪口后坐力", meta = (DisplayPriority = "3", DisplayName = "方向冲击随机范围 (度)", ToolTip = "X 上下 Y 左右 每枪在各轴的负范围至正范围内随机 越大波动越大 0 无随机"))
    FVector2D AimImpulseRandomDegrees = FVector2D(0.25f, 0.5f);

    /** 枪口额外角度偏移的恢复速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "枪口后坐力", meta = (DisplayPriority = "4", ClampMin = "0.01", DisplayName = "方向冲击回零速率 (1/秒)", ToolTip = "指数衰减系数 越大方向偏移回零越快 不改变目标跟随速度 与镜头回零独立"))
    float AimImpulseRecoverySpeed = 14.0f;

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
