#pragma once

#include "CoreMinimal.h"
#include "Navigation/APS_MoveToInterface.h"
#include "BBBMonsterNavigationProfile.generated.h"

/** 一种行为的 APS 路线形状配置 使用出生种子稳定抽样 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterNavigationProfile final
{
    GENERATED_BODY()

    /** 弧度绝对值范围 正负方向由个体偏好确定 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "路线形状", meta = (DisplayName = "弧度强度范围", ToolTip = "相对强度 不是米数 同一实例重算时保持稳定", ClampMin = "0", ClampMax = "3"))
    FVector2D ArcStrengthRange = FVector2D(0.2, 0.7);

    /** 关闭弧线和横向偏移但仍使用 APS 的个体比例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "路线形状", meta = (DisplayName = "直接接近比例", ClampMin = "0", ClampMax = "1"))
    float DirectPathChance = 0.2f;

    /** 个体选择正向弧线的概率 另一方向使用负值 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "路线形状", meta = (DisplayName = "正向绕行比例", ClampMin = "0", ClampMax = "1"))
    float PositiveArcChance = 0.5f;

    /** 同一实例重算时使用稳定的横向偏移 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "路线形状", meta = (DisplayName = "横向偏移范围", ToolTip = "单位为厘米 大幅偏移可能触发额外侧向寻路", Units = "cm", ClampMin = "-300", ClampMax = "300"))
    FVector2D LaneOffsetRange = FVector2D(-50.0, 50.0);

    /** 平滑弧线用于常规追击 波浪可用于特殊巡逻风格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "路线形状", meta = (DisplayName = "曲线模式"))
    EAPSArcMidpointMode MidpointMode = EAPSArcMidpointMode::SmoothCurve;

    /** 控制曲线形状 不等于最终路径点数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "路线形状", meta = (DisplayName = "曲线形状参数", ClampMin = "1", ClampMax = "6"))
    int32 NumMidPoints = 2;

    /** @return 配置中的范围 概率与枚举是否有效 */
    bool IsValid() const
    {
        return !ArcStrengthRange.ContainsNaN() && ArcStrengthRange.X >= 0.0 && ArcStrengthRange.Y >= ArcStrengthRange.X
            && ArcStrengthRange.Y <= 3.0 && !LaneOffsetRange.ContainsNaN() && LaneOffsetRange.X >= -300.0
            && LaneOffsetRange.Y >= LaneOffsetRange.X && LaneOffsetRange.Y <= 300.0
            && FMath::IsFinite(DirectPathChance) && DirectPathChance >= 0.0f && DirectPathChance <= 1.0f
            && FMath::IsFinite(PositiveArcChance) && PositiveArcChance >= 0.0f && PositiveArcChance <= 1.0f
            && NumMidPoints >= 1 && NumMidPoints <= 6
            && (MidpointMode == EAPSArcMidpointMode::SmoothCurve || MidpointMode == EAPSArcMidpointMode::Wavy);
    }

    /**
     * @param Seed	个体稳定出生种子
     * @param ArcStrength	输出带方向的弧线强度
     * @param LaneOffset	输出横向偏移
     * @return 无返回值 同一种子与配置始终得到相同结果
     */
    void Sample(const uint32 Seed, float& ArcStrength, float& LaneOffset) const
    {
        FRandomStream Random(static_cast<int32>(HashCombine(Seed, 0x74d2b519u)));
        const bool bDirect = Random.FRand() < DirectPathChance;
        const float Side = Random.FRand() < PositiveArcChance ? 1.0f : -1.0f;
        ArcStrength = bDirect ? 0.0f : Side * Random.FRandRange(ArcStrengthRange.X, ArcStrengthRange.Y);
        LaneOffset = bDirect ? 0.0f : Random.FRandRange(LaneOffsetRange.X, LaneOffsetRange.Y);
    }
};
