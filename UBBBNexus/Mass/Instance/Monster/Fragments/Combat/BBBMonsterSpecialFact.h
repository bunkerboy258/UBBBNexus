#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterSpecialFact.generated.h"

/** 当前特感动作 阶段零为空 一蓄力 二冲撞 三恢复 四喷吐 五潜行 六狂攻 七冲撞受阻 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterSpecialFact final
{
    GENERATED_BODY()

    /** 本代怪物的特殊动作递增编号 */
    UPROPERTY()
    uint32 Serial = 0;

    /** 当前阶段编号 */
    UPROPERTY(BlueprintReadOnly)
    uint8 Phase = 0;

    /** 当前阶段的世界时间起点 */
    UPROPERTY(BlueprintReadOnly)
    float StartedAt = 0;

    /** 当前阶段的预定时长 */
    UPROPERTY(BlueprintReadOnly)
    float Duration = 0;

    /** 冲撞方向或喷吐目标 锁定后不随玩家移动追踪 */
    UPROPERTY(BlueprintReadOnly)
    FVector Goal = FVector::ZeroVector;

    /** 当前冲撞已完成的实际水平距离 厘米 */
    float RushDistanceCm = 0;

    /** @return 当前阶段是否接管正常移动 */
    bool HoldsMovement() const
    {
        return (Phase >= 1 && Phase <= 4) || Phase == 7;
    }

    /** @return 当前字段是否可用于网络与表现 */
    bool IsValid() const
    {
        return Phase <= 7 && FMath::IsFinite(StartedAt) && FMath::IsFinite(Duration)
            && Duration >= 0 && !Goal.ContainsNaN() && FMath::IsFinite(RushDistanceCm) && RushDistanceCm >= 0;
    }
};
