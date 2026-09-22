#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "MonsterStateData.generated.h"

/** 小怪当前的权威逻辑状态 */
UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    /** 未发现目标时的待机状态 */
    Idle,

    /** 等待玩家进入可侦察范围 */
    Scout,

    /** 沿导航路径接近目标 */
    Chase,

    /** 处于攻击距离内 */
    Attack,

    /** 收到伤害后的短暂硬直 */
    Hurt,

    /** 生命值归零后的死亡状态 */
    Dead
};

/** 标记实体属于小怪 */
USTRUCT()
struct ABBB_EVAC_API FMonsterTag final : public FMassTag
{
    GENERATED_BODY()
};

/** 小怪生命值状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterHealthFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float CurrentHealth = 100.0f;

    /** 最大生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float MaxHealth = 100.0f;

    /** 每次受伤重新计算的硬直时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster", meta = (ClampMin = "0.01"))
    float HurtDuration = 0.2f;

    /** 死亡表现保留到实体回收的时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster", meta = (ClampMin = "0.01"))
    float DeathLifetime = 3.0f;
};

/** 小怪行为状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterStateFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前权威状态 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    EMonsterState State = EMonsterState::Idle;

    /** 当前状态进入的世界时间 */
    float StateEnteredTime = 0.0f;

    /** 状态切换或同状态动作重启时递增 */
    uint32 ActionId = 0;
};

/** 小怪移动配置与运行状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterMovementFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 最大移动速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float MoveSpeed = 300.0f;

    /** 进入攻击状态的停止半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float StopRadius = 120.0f;

    /** 下次允许刷新导航路径的时间 */
    float NextPathRefreshTime = 0.0f;

    /** 当前导航路径上的下一点 */
    FVector NextPathPoint = FVector::ZeroVector;
};

/** 小怪侦察配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterPerceptionFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 能够发现玩家的最大距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float SightRange = 3500.0f;
};

/** 小怪局部避让配置与运行状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterAvoidanceFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 小怪球形碰撞体半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float CollisionRadius = 45.0f;

    /** 小怪之间保持的最小中心距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float PersonalSpaceRadius = 110.0f;

    /** 参与局部避让计算的邻居范围 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float NeighborSearchRadius = 180.0f;

    /** 避让方向对追击方向的影响权重 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float AvoidanceWeight = 2.0f;

    /** 当前帧计算出的远离邻居方向 */
    FVector SeparationDirection = FVector::ZeroVector;

    /** 当前帧计算出的避让强度 */
    float SeparationStrength = 0.0f;
};

/** 小怪攻击配置与运行状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterCombatFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 允许攻击的最大距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float AttackRange = 180.0f;

    /** 单次攻击伤害 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float AttackDamage = 10.0f;

    /** 两次攻击之间的最短时间 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float AttackCooldown = 1.2f;

    /** 开始攻击到唯一命中判定之间的时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster", meta = (ClampMin = "0.01"))
    float AttackWindup = 0.35f;

    /** 命中判定到本轮攻击结束之间的时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster", meta = (ClampMin = "0.01"))
    float AttackRecovery = 0.45f;

    /** 攻击动画中对应命中判定的归一化位置 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster", meta = (ClampMin = "0.01", ClampMax = "0.99"))
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

/** 小怪表现层只读状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterPresentationStateFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 提供给表现层的状态 */
    EMonsterState State = EMonsterState::Idle;

    /** 提供给表现层的移动速度 */
    float Speed = 0.0f;

    /** 提供给表现层的状态进入时间 */
    float StateEnteredTime = 0.0f;

    /** 对应逻辑动作的独立编号 */
    uint32 ActionId = 0;

    /** 非循环动作的归一化动画位置 */
    float ActionProgress = 0.0f;
};
