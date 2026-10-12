#pragma once

#include "CoreMinimal.h"
#include "BBBRevolverCrouchedModifiers.generated.h"

/** 转轮手枪下蹲时叠加在当前腰射或瞄准配置上的倍率 全部为 1 时不变 */
USTRUCT(BlueprintType)
struct FBBBRevolverCrouchedModifiers
{
    GENERATED_BODY()

    /** 下蹲目标跟随倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "目标跟随", meta = (ClampMin = "0.01", DisplayName = "目标跟随倍率", ToolTip = "无单位 1 不变 大于 1 跟随更快 小于 1 跟随更慢"))
    float AimFollowScale = 1.0f;

    /** 下蹲枪口冲击倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "枪口后坐力", meta = (ClampMin = "0.00", DisplayName = "枪口冲击倍率", ToolTip = "无单位 1 不变 越大每枪上下左右冲击及随机幅度越大 0 无方向冲击"))
    float AimImpulseScale = 1.0f;

    /** 下蹲枪口回零速度倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "枪口后坐力", meta = (ClampMin = "0.01", DisplayName = "枪口回零速度倍率", ToolTip = "无单位 1 不变 越大回零越快 小于 1 回零更慢 只在开火时选择"))
    float AimImpulseRecoveryScale = 1.0f;

    /** 下蹲后向震动倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "后向震动", meta = (ClampMin = "0.00", DisplayName = "后向震动倍率", ToolTip = "无单位 1 不变 越大向后震动越强 0 无震动 最终动画权重限制为 0 到 1"))
    float BackwardRecoilScale = 1.0f;

    /** 下蹲摇摆幅度倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "持枪摇摆", meta = (ClampMin = "0.00", DisplayName = "摇摆幅度倍率", ToolTip = "无单位 1 不变 越大上下左右摇摆幅度越大 0 无摇摆"))
    float SwayAmplitudeScale = 1.0f;

    /** 下蹲摇摆频率倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "持枪摇摆", meta = (ClampMin = "0.00", DisplayName = "摇摆频率倍率", ToolTip = "无单位 1 不变 越大摇摆越快 0 使该摇摆分量为零"))
    float SwayFrequencyScale = 1.0f;

    /** 下蹲镜头冲击倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "镜头后坐力", meta = (ClampMin = "0.00", DisplayName = "镜头冲击倍率", ToolTip = "无单位 1 不变 越大每枪镜头冲击及随机幅度越大 0 无镜头冲击"))
    float CameraImpulseScale = 1.0f;

    /** 下蹲镜头回零速度倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "镜头后坐力", meta = (ClampMin = "0.01", DisplayName = "镜头回零速度倍率", ToolTip = "无单位 1 不变 越大镜头回零越快 小于 1 回零更慢"))
    float CameraRecoveryScale = 1.0f;
};
