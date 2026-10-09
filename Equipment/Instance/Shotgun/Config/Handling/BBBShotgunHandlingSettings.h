#pragma once

#include "CoreMinimal.h"
#include "BBBShotgunHandlingSettings.generated.h"

/** 霰弹枪在一种持枪姿态下提供的表现参数 */
USTRUCT(BlueprintType)
struct FBBBShotgunHandlingSettings
{
    GENERATED_BODY()

    /** 枪口追随目标的响应速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "跟随", meta = (ClampMin = "0.01", DisplayName = "目标跟随速度", ToolTip = "数值越大枪口越快追上瞄准目标 不控制开火冲击回零"))
    float AimFollowSpeed = 18.0f;

    /** 每枪向上和向右的角度冲击 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "方向后坐力", meta = (DisplayName = "枪口冲量 角度", ToolTip = "X 为向上 Y 为向右 单位为度 负值表示相反方向"))
    FVector2D AimImpulseDegrees = FVector2D(2.0f, 0.15f);

    /** 每枪方向冲击的对称随机范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "方向后坐力", meta = (DisplayName = "枪口冲量随机范围", ToolTip = "上下与左右分别在负范围到正范围内随机 单位为度"))
    FVector2D AimImpulseRandomDegrees = FVector2D(0.25f, 0.5f);

    /** 枪口额外角度偏移的恢复速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "方向后坐力", meta = (ClampMin = "0.01", DisplayName = "枪口冲击回零速度", ToolTip = "由角色维护 数值越大开火产生的额外角度越快恢复 不影响镜头"))
    float AimImpulseRecoverySpeed = 14.0f;

    /** 向后受力动画的混合强度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "向后后坐力", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "向后震动 Alpha", ToolTip = "缩放角色向后受力动画 1 为参考动画完整幅度 每次开火从头播放"))
    float BackwardRecoilAlpha = 0.45f;

    /** 连续持枪摇摆的上下与左右幅度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "摇摆", meta = (DisplayName = "摇摆幅度 角度", ToolTip = "X 为上下 Y 为左右的最大角度 影响实际枪口方向"))
    FVector2D SwayAmplitudeDegrees = FVector2D(0.25f, 0.35f);

    /** 连续持枪摇摆的上下与左右频率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "摇摆", meta = (DisplayName = "摇摆频率 赫兹", ToolTip = "X 为上下 Y 为左右每秒往复次数 0 表示该轴不摇摆"))
    FVector2D SwayFrequency = FVector2D(0.65f, 0.43f);
};
