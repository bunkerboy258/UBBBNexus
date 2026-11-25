#pragma once

#include "CoreMinimal.h"
#include "BBBSMGStationaryModifiers.generated.h"

/** 冲锋枪原地不动时叠加在当前腰射或瞄准配置上的倍率 可与下蹲叠加 全部为 1 时不变 */
USTRUCT(BlueprintType)
struct FBBBSMGStationaryModifiers
{
    GENERATED_BODY()

    /** 原地轨迹幅度倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "枪口后坐力", meta = (DisplayPriority = "1", ClampMin = "0.00", DisplayName = "轨迹幅度倍率", ToolTip = "无单位 1 不变 小于 1 抑制原地后坐力轨迹 0 无轨迹偏移 可与下蹲倍率相乘"))
    float RecoilScale = 0.8f;

    /** 原地后向震动倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "后向震动", meta = (DisplayPriority = "2", ClampMin = "0.00", DisplayName = "后向震动倍率", ToolTip = "无单位 1 不变 小于 1 抑制原地后向震动 0 无震动 最终动画权重限制为 0 到 1"))
    float BackwardRecoilScale = 0.8f;

    /** 原地镜头冲击倍率 无单位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "镜头后坐力", meta = (DisplayPriority = "3", ClampMin = "0.00", DisplayName = "镜头冲击倍率", ToolTip = "无单位 1 不变 小于 1 抑制原地每枪镜头冲击及随机幅度 0 无镜头冲击"))
    float CameraImpulseScale = 0.8f;
};
