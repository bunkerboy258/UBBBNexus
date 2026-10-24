#pragma once

#include "BBBMonsterAnimationPresentation.generated.h"

/** 动画采样参数 不改变实际速度和行为时机 */
USTRUCT(BlueprintType)
struct FBBBMonsterAnimationPresentation final
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (DisplayName = "移动风格权重"))
    FVector LocomotionWeights = FVector(1, 1, 1);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "随机初始相位"))
    bool bRandomPhase = true;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "动画步幅比例", ClampMin = "0.1"))
    float AnimationStrideScale = 1;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "死亡动画秒数", ClampMin = "0.1"))
    float DeathAnimationDuration = 1.7f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "尸体活动物理秒数", ClampMin = "0.5", ClampMax = "10"))
    float CorpseSimulationDuration = 4;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "攻击命中动画进度", ClampMin = "0.01", ClampMax = "0.99"))
    float AnimationHitFraction = 0.4f;

    UPROPERTY(EditAnywhere, EditFixedSize, meta = (DisplayName = "三种风格走跑冲刺循环秒数"))
    TArray<FVector> MovementCycleSeconds = {FVector(1, 1, 1), FVector(1, 1, 1), FVector(1, 1, 1)};

    UPROPERTY(EditAnywhere, meta = (DisplayName = "蹒跚采样速度", ClampMin = "1"))
    float SlowWalkSampleSpeed = 40;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "蹒跚循环秒数"))
    FVector SlowWalkCycleSeconds = FVector(1, 1, 1);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "追近快走循环秒数"))
    FVector PursuitWalkCycleSeconds = FVector(0.766667, 0.766667, 0.766667);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "待机循环秒数"))
    FVector IdleCycleSeconds = FVector(1, 1, 1);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "警觉循环秒数", ClampMin = "0.01"))
    float AlertCycleSeconds = 1;
};
