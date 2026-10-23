#pragma once

#include "CoreMinimal.h"
#include "BBBLMGAirborneModifiers.generated.h"

/** 腾空时叠加在腰射或瞄准基础参数上的倍率 */
USTRUCT(BlueprintType)
struct FBBBLMGAirborneModifiers
{
    GENERATED_BODY()

    /** 空中目标跟随速度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "目标跟随", meta = (DisplayPriority = "1", ClampMin = "0.01", DisplayName = "目标跟随倍率", ToolTip = "无单位 1 不变 大于 1 空中跟随更快 小于 1 更慢"))
    float AimFollowScale = 0.75f;

    /** 空中轨迹偏移倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "枪口后坐力", meta = (DisplayPriority = "2", ClampMin = "0.0", DisplayName = "轨迹偏移倍率", ToolTip = "无单位 1 不变 越大空中每枪轨迹偏移及随机幅度越大 0 无轨迹偏移 不缩放回零速率"))
    float RecoilScale = 1.35f;

    /** 空中向后受力强度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "后向震动", meta = (DisplayPriority = "3", ClampMin = "0.0", DisplayName = "后向震动倍率", ToolTip = "无单位 1 不变 越大空中震动越强 0 无震动 最终动画权重限制为 0 到 1"))
    float BackwardRecoilScale = 1.2f;

    /** 空中摇摆幅度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "持枪摇摆", meta = (DisplayPriority = "4", ClampMin = "0.0", DisplayName = "摇摆幅度倍率", ToolTip = "无单位 1 不变 越大空中摇摆幅度越大 0 无摇摆"))
    float SwayAmplitudeScale = 1.8f;

    /** 空中摇摆频率倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "持枪摇摆", meta = (DisplayPriority = "5", ClampMin = "0.0", DisplayName = "摇摆频率倍率", ToolTip = "无单位 1 不变 越大空中摇摆越快 0 使摇摆分量为零"))
    float SwayFrequencyScale = 1.25f;
};
