#pragma once

#include "Engine/DataAsset.h"
#include "BBBMonsterVariationDefinition.generated.h"

/** 共享出生个体化规则 不在存活期间重新抽样 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterVariationDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 最低感染度的跑尸概率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|出生差异", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "低感染跑尸概率"))
    float LowInfectionRunnerChance = 0.38f;

    /** 最高感染度的跑尸概率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|出生差异", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "高感染跑尸概率"))
    float HighInfectionRunnerChance = 0.02f;

    /** 三种步态共享一次出生速度偏差 保持速度顺序 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|出生差异", meta = (ClampMin = "0.0", ClampMax = "0.3", DisplayName = "速度偏差幅度"))
    float SpeedVariation = 0.15f;

    /** 出生时确定的加速度偏差 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|出生差异", meta = (ClampMin = "0.0", ClampMax = "0.3", DisplayName = "加速度偏差幅度"))
    float AccelerationVariation = 0.15f;

    /** 警觉停留时长的相对偏差 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|出生差异", meta = (ClampMin = "0.0", ClampMax = "0.3", DisplayName = "警觉时长偏差"))
    float AlertVariation = 0.2f;

    /** 三种移动风格的走 跑 冲刺循环秒数 由正式样本写入 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|循环采样", meta = (DisplayName = "移动风格循环秒数"))
    TArray<FVector> MovementCycleSeconds = {FVector(1.0, 1.0, 1.0), FVector(1.0, 1.0, 1.0), FVector(1.0, 1.0, 1.0)};

    /** 三种身份选择待机的循环秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|循环采样", meta = (DisplayName = "待机循环秒数"))
    FVector IdleCycleSeconds = FVector(1.0, 1.0, 1.0);

    /** 警觉循环的正式样本秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|循环采样", meta = (ClampMin = "0.01", DisplayName = "警觉循环秒数"))
    float AlertCycleSeconds = 1.0f;

    /** @return 全部出生规则是否有效 */
    bool IsValid() const;
};
