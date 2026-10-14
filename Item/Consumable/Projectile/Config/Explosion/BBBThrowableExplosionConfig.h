#pragma once
#include "CoreMinimal.h"
#include "BBBThrowableExplosionConfig.generated.h"

/** 投掷物Explosion静态配置 */
USTRUCT(BlueprintType)
struct FBBBThrowableExplosionConfig final
{
    GENERATED_BODY()

    /** 从实体释放开始计时 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "引信时间", ToolTip = "从实体释放开始计时 单位秒", ClampMin = "0", ForceUnits = "s"))
    float FuseSeconds = 3.0f;

    /** 开启时物理碰撞触发爆炸 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "接触引爆", ToolTip = "开启时物理碰撞触发爆炸"))
    bool bOnImpact = false;

    /** 超时清理实体 不额外引爆 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "最大存活时间", ToolTip = "超时清理实体 不额外引爆", ClampMin = "0.1", ForceUnits = "s"))
    float MaxLifetime = 15.0f;

    /** 爆炸最大伤害范围 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "伤害半径", ToolTip = "爆炸最大伤害范围 单位厘米", ClampMin = "1", ForceUnits = "cm"))
    float Radius = 500.0f;

    /** 爆炸中心的普通伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "普通伤害", ToolTip = "爆炸中心的普通伤害", ClampMin = "0"))
    float Damage = 120.0f;

    /** 高耐久部位参与混合的伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "耐久伤害", ToolTip = "高耐久部位参与混合的伤害", ClampMin = "0"))
    float DurableDamage = 80.0f;

    /** 伤害半径边缘保留的伤害倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "边缘伤害倍率", ToolTip = "伤害半径边缘保留的伤害倍率", ClampMin = "0", ClampMax = "1"))
    float MinimumDamageRatio = 0.2f;

    /** 一次爆炸每个目标的踉跄概率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "踉跄概率", ToolTip = "一次爆炸每个目标的踉跄概率", ClampMin = "0", ClampMax = "1"))
    float Stagger = 0.8f;

    /** 世界静态遮挡阻止范围伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "检查遮挡", ToolTip = "世界静态遮挡阻止范围伤害"))
    bool bCheckOcclusion = true;

    /** 爆炸可以伤害投掷者 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "伤害投掷者", ToolTip = "爆炸可以伤害投掷者"))
    bool bDamageSelf = true;

    /** 爆炸可以伤害其他角色 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "伤害其他玩家", ToolTip = "爆炸可以伤害其他角色"))
    bool bDamagePlayers = true;

    /** 物理冲击范围 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "冲击半径", ToolTip = "物理冲击范围 单位厘米", ClampMin = "0", ForceUnits = "cm"))
    float ImpulseRadius = 650.0f;

    /** 径向物理冲击强度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "冲击强度", ToolTip = "径向物理冲击强度", ClampMin = "0"))
    float ImpulseStrength = 1800.0f;

    /** 开启后按速度变化施加冲击 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "冲击忽略质量", ToolTip = "开启后按速度变化施加冲击"))
    bool bVelocityChange = true;
};
