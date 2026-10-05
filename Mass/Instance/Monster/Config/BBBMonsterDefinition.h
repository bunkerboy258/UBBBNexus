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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (DisplayName = "实体配置"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig;

    /** 最大生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "最大生命值"))
    float MaxHealth = 100.0f;

    /** 受伤硬直秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "受击持续时间"))
    float HurtDuration = 0.2f;

    /** 死亡表现保留秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "死亡后存活时间"))
    float DeathLifetime = 3.0f;

    /** 移动速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "移动速度"))
    float MoveSpeed = 300.0f;

    /** 接近目标停止距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "停止半径"))
    float StopRadius = 120.0f;

    /** 感知范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "视野范围"))
    float SightRange = 3500.0f;

    /** 逻辑球形碰撞半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "碰撞半径"))
    float CollisionRadius = 45.0f;

    /** 个体间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "个人空间半径"))
    float PersonalSpaceRadius = 110.0f;

    /** 邻居查询范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "邻近搜索半径"))
    float NeighborSearchRadius = 180.0f;

    /** 避让权重 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "避让权重"))
    float AvoidanceWeight = 2.0f;

    /** 攻击距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击范围"))
    float AttackRange = 180.0f;

    /** 攻击伤害 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击伤害"))
    float AttackDamage = 10.0f;

    /** 攻击间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击冷却时间"))
    float AttackCooldown = 1.2f;

    /** 攻击命中前摇 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击前摇时间"))
    float AttackWindup = 0.35f;

    /** 攻击后摇 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击恢复时间"))
    float AttackRecovery = 0.45f;

    /** 命中对应动画进度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "动画命中时间比例"))
    float AnimationHitFraction = 0.4f;


    /** @return 参数是否合法 */
    bool IsValid() const;
};
