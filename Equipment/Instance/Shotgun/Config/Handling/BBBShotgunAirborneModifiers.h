#pragma once

#include "CoreMinimal.h"
#include "BBBShotgunAirborneModifiers.generated.h"

/** 腾空时叠加在腰射或瞄准基础参数上的倍率 */
USTRUCT(BlueprintType)
struct FBBBShotgunAirborneModifiers
{
    GENERATED_BODY()

    /** 空中目标跟随速度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "空中修正", meta = (ClampMin = "0.01", DisplayName = "跟随速度倍率", ToolTip = "乘到当前腰射或瞄准的跟随速度 小于 1 时空中跟随更慢"))
    float AimFollowScale = 0.75f;

    /** 空中方向冲击倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "空中修正", meta = (ClampMin = "0.0", DisplayName = "方向冲量倍率", ToolTip = "乘到当前上下左右冲量及随机范围"))
    float AimImpulseScale = 1.35f;

    /** 空中向后受力强度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "空中修正", meta = (ClampMin = "0.0", DisplayName = "向后震动倍率", ToolTip = "乘到向后动画 Alpha 最终限制在 0 到 1"))
    float BackwardRecoilScale = 1.2f;

    /** 空中摇摆幅度倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "空中修正", meta = (ClampMin = "0.0", DisplayName = "摇摆幅度倍率", ToolTip = "乘到当前上下左右摇摆幅度"))
    float SwayAmplitudeScale = 1.8f;

    /** 空中摇摆频率倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "空中修正", meta = (ClampMin = "0.0", DisplayName = "摇摆频率倍率", ToolTip = "乘到当前上下左右摇摆频率"))
    float SwayFrequencyScale = 1.25f;
};
