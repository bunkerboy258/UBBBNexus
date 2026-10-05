#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterCombatFragment.generated.h"

/** 小怪攻击配置与运行状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterCombatFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 允许攻击的最大距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "攻击范围"))
    float AttackRange = 180.0f;

    /** 单次攻击伤害 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "攻击伤害"))
    float AttackDamage = 10.0f;

    /** 两次攻击之间的最短时间 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "攻击冷却时间"))
    float AttackCooldown = 1.2f;

    /** 开始攻击到唯一命中判定之间的时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (ClampMin = "0.01", DisplayName = "攻击前摇时间"))
    float AttackWindup = 0.35f;

    /** 命中判定到本轮攻击结束之间的时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (ClampMin = "0.01", DisplayName = "攻击恢复时间"))
    float AttackRecovery = 0.45f;

    /** 攻击动画中对应命中判定的归一化位置 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (ClampMin = "0.01", ClampMax = "0.99", DisplayName = "动画命中时间比例"))
    float AnimationHitFraction = 0.4f;

    /** 下次允许攻击的时间 */
    float NextAttackTime = 0.0f;

    /** 每轮攻击独立编号 受伤取消后不复用 */
    uint32 AttackId = 0;

    /** 当前攻击锁定的目标 不随感知切换 */
    TWeakObjectPtr<AActor> AttackTarget;

    /** 本轮是否已经执行或取消唯一命中判定 */
    bool bHitAttempted = false;

    /** 战斗处理器确认本轮时序已经结束 */
    bool bAttackFinished = false;
};
