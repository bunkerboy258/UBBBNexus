#pragma once

#include "Engine/DataAsset.h"
#include "BBBMonsterDefinition.generated.h"

class UMassEntityConfigAsset;

/** 小怪模板与静态玩法参数 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 还原出生时使用的实体模板 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster")
    TObjectPtr<UMassEntityConfigAsset> EntityConfig;

    /** 最大生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float MaxHealth = 100.0f;

    /** 受伤硬直秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float HurtDuration = 0.2f;

    /** 死亡表现保留秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float DeathLifetime = 3.0f;

    /** 移动速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float MoveSpeed = 300.0f;

    /** 接近目标停止距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float StopRadius = 120.0f;

    /** 感知范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float SightRange = 3500.0f;

    /** 逻辑球形碰撞半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float CollisionRadius = 45.0f;

    /** 个体间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float PersonalSpaceRadius = 110.0f;

    /** 邻居查询范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float NeighborSearchRadius = 180.0f;

    /** 避让权重 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float AvoidanceWeight = 2.0f;

    /** 攻击距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float AttackRange = 180.0f;

    /** 攻击伤害 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float AttackDamage = 10.0f;

    /** 攻击间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float AttackCooldown = 1.2f;

    /** 攻击命中前摇 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float AttackWindup = 0.35f;

    /** 攻击后摇 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float AttackRecovery = 0.45f;

    /** 命中对应动画进度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|Monster", meta = (ClampMin = "0.0"))
    float AnimationHitFraction = 0.4f;


    /** @return 参数是否合法 */
    bool IsValid() const;
};
