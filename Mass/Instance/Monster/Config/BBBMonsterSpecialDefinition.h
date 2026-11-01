#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterSpecialKind.h"
#include "BBBMonsterSpecialDefinition.generated.h"

class UBBBSlimeDefinition;

/** 保存在怪物逻辑配置内部的特感规则 */
UCLASS(EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBMonsterSpecialDefinition final : public UObject
{
    GENERATED_BODY()

public:
    /** 本模板启用的特殊能力 */
    UPROPERTY(EditAnywhere, Category = "特殊感染者", meta = (DisplayName = "能力种类"))
    EBBBMonsterSpecialKind Kind = EBBBMonsterSpecialKind::None;

    /** 特殊动作两次开始之间的最短时间 */
    UPROPERTY(EditAnywhere, Category = "特殊感染者", meta = (DisplayName = "技能冷却", Units = "s", ClampMin = "0.1"))
    float Cooldown = 8.0f;

    /** 冲撞或喷吐的准备时长 */
    UPROPERTY(EditAnywhere, Category = "特殊感染者", meta = (DisplayName = "技能前摇", Units = "s", ClampMin = "0.1"))
    float Windup = 1.0f;

    /** 动作结束后的恢复时长 */
    UPROPERTY(EditAnywhere, Category = "特殊感染者", meta = (DisplayName = "技能恢复", Units = "s", ClampMin = "0.1"))
    float Recovery = 1.5f;

    /** 技能开始时的最近距离 */
    UPROPERTY(EditAnywhere, Category = "特殊感染者", meta = (DisplayName = "最小施放距离", Units = "cm", ClampMin = "0"))
    float MinimumRange = 350.0f;

    /** 技能开始时的最远距离 */
    UPROPERTY(EditAnywhere, Category = "特殊感染者", meta = (DisplayName = "最大施放距离", Units = "cm", ClampMin = "1"))
    float MaximumRange = 1500.0f;

    /** 冲撞与突袭的移动速度 */
    UPROPERTY(EditAnywhere, Category = "冲撞与突袭", meta = (DisplayName = "突进速度", Units = "cm/s", ClampMin = "1"))
    float RushSpeed = 1000.0f;

    /** 冲撞的最长推进距离 */
    UPROPERTY(EditAnywhere, Category = "冲撞与突袭", meta = (DisplayName = "最大冲撞距离", Units = "cm", ClampMin = "1"))
    float RushDistance = 1800.0f;

    /** 冲撞命中的生命伤害 */
    UPROPERTY(EditAnywhere, Category = "冲撞与突袭", meta = (DisplayName = "冲撞伤害", ClampMin = "0"))
    float RushDamage = 50.0f;

    /** 命中后玩家的水平击飞速度 */
    UPROPERTY(EditAnywhere, Category = "冲撞与突袭", meta = (DisplayName = "水平击飞速度", Units = "cm/s", ClampMin = "0", ClampMax = "2500"))
    float ImpactSpeed = 900.0f;

    /** 命中后玩家的向上击飞速度 */
    UPROPERTY(EditAnywhere, Category = "冲撞与突袭", meta = (DisplayName = "向上击飞速度", Units = "cm/s", ClampMin = "0", ClampMax = "1200"))
    float ImpactLift = 350.0f;

    /** 玩家本轮布娃娃的建议最短持续时间 */
    UPROPERTY(EditAnywhere, Category = "冲撞与突袭", meta = (DisplayName = "布娃娃持续时间", Units = "s", ClampMin = "0.1", ClampMax = "10"))
    float KnockdownDuration = 2.0f;

    /** 潜行候选点围绕玩家的距离 */
    UPROPERTY(EditAnywhere, Category = "潜行", meta = (DisplayName = "潜行接近半径", Units = "cm", ClampMin = "100"))
    float StalkRadius = 650.0f;

    /** 进入此距离后冲出攻击 */
    UPROPERTY(EditAnywhere, Category = "潜行", meta = (DisplayName = "突袭触发距离", Units = "cm", ClampMin = "100"))
    float AmbushRange = 650.0f;

    /** 寻找潜行位置的刷新间隔 */
    UPROPERTY(EditAnywhere, Category = "潜行", meta = (DisplayName = "潜行选点间隔", Units = "s", ClampMin = "0.1"))
    float StalkInterval = 1.0f;

    /** 狂攻阶段的持续时间 */
    UPROPERTY(EditAnywhere, Category = "潜行", meta = (DisplayName = "狂攻持续时间", Units = "s", ClampMin = "0.1"))
    float FrenzyDuration = 5.0f;

    /** 狂攻阶段近战间隔相对基础攻击间隔的比例 */
    UPROPERTY(EditAnywhere, Category = "潜行", meta = (DisplayName = "狂攻间隔比例", ClampMin = "0.1", ClampMax = "1"))
    float FrenzyCooldownScale = 0.45f;

    /** 飞行 地面区域与接触效果的唯一配置 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "粘液配置"))
    TObjectPtr<UBBBSlimeDefinition> Slime;

    /** @return 当前能力参数是否有限且处于有效范围 */
    bool IsValid() const;
};
