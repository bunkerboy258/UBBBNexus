#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Camera/BBBCharacterCameraConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Interaction/BBBCharacterInteractionConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Network/BBBNetworkConfig.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Appearance/BBBCharacterAppearanceConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Pickup/BBBCharacterPickupConfig.h"
#include "BBBCharacterConfig.generated.h"

/** 角色全部可编辑的静态配置资产 */
UCLASS(BlueprintType)
//聚合角色全部可编辑运行配置
class ABBB_EVAC_API UBBBCharacterConfig final : public UPrimaryDataAsset
{
    GENERATED_BODY()
  public:
    UBBBCharacterConfig();

    /** 世界掉落物自动拾取配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "拾取配置"))
    FBBBCharacterPickupConfig Pickup;
  public:
    /** 快速近战普通伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "快速近战伤害", ClampMin = "0"))
    float QuickMeleeDamage = 45.0f;
    /** 快速近战耐久伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "快速近战耐久伤害", ClampMin = "0"))
    float QuickMeleeDurableDamage = 20.0f;
    /** 快速近战贡献的踉跄强度 当前僵尸各部位都达到满额概率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "近战", meta = (DisplayName = "快速近战踉跄强度", ClampMin = "0"))
    float QuickMeleeStaggerValue = 200.0f;
    /** 快速近战沿角色前方贡献的速度增量 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "近战", meta = (DisplayName = "快速近战推力速度", ClampMin = "0", ForceUnits = "cm/s"))
    float QuickMeleeImpactSpeed = 400.0f;
    /** 快速近战贡献的冲击保持时间 由目标解释 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "近战", meta = (DisplayName = "快速近战冲击保持时间", ClampMin = "0", ForceUnits = "s"))
    float QuickMeleeImpactHoldSeconds = 1.0f;

    /** 手部接触段半径 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "快速近战接触半径", ClampMin = "1"))
    float QuickMeleeRadius = 14.0f;
    /** 从手部向前延伸的接触段 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "快速近战接触长度", ClampMin = "1"))
    float QuickMeleeReach = 45.0f;

  public:
    /** 正常生命上限与出生生命 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命", meta = (DisplayName = "最大生命", ClampMin = "1"))
    float MaximumHealth = 150.0f;

    /** 护甲上限与出生护甲 优先承受伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|护甲", meta = (DisplayName = "最大护甲", ClampMin = "0"))
    float MaximumArmor = 250.0f;
    /** 受到伤害后开始自然回复护甲前的等待秒数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|护甲", meta = (DisplayName = "护甲回复延迟", ClampMin = "0", ForceUnits = "s"))
    float ArmorRegenDelaySeconds = 3.0f;
    /** 脱战后每秒自然回复护甲 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|护甲", meta = (DisplayName = "护甲每秒回复", ClampMin = "0"))
    float ArmorRegenPerSecond = 35.0f;

    /** 体力上限与出生体力 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "体力", meta = (DisplayName = "最大体力", ClampMin = "0"))
    float MaximumStamina = 150.0f;
    /** 站立冲刺每秒消耗 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "体力", meta = (DisplayName = "冲刺每秒消耗", ClampMin = "0"))
    float SprintDrainPerSecond = 12.0f;
    /** 滑铲每秒消耗 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "体力", meta = (DisplayName = "滑铲每秒消耗", ClampMin = "0"))
    float SlideDrainPerSecond = 10.0f;
    /** 快速近战与装备近战每次攻击消耗 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "体力", meta = (DisplayName = "近战攻击消耗", ClampMin = "0"))
    float MeleeAttackCost = 8.0f;
    /** 无消耗后每秒回复 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "体力", meta = (DisplayName = "每秒回复", ClampMin = "0"))
    float RegenPerSecond = 30.0f;
    /** 无消耗后开始回复前的等待秒数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "体力", meta = (DisplayName = "回复延迟", ClampMin = "0", ForceUnits = "s"))
    float RegenDelaySeconds = 0.5f;

    /** 从本次最高点下降到此距离时在空中进入布娃娃 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|跌落", meta = (DisplayName = "跌落布娃娃距离", ClampMin = "1"))
    float FallRagdollDistance = 700.0f;
    /** 首次撞地向下速度不超过此值时不受伤 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|跌落", meta = (DisplayName = "跌落无伤速度", ClampMin = "0"))
    float FallSafeSpeed = 950.0f;
    /** 首次撞地向下速度达到此值时使用满额伤害 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|跌落", meta = (DisplayName = "跌落满额伤害速度", ClampMin = "1"))
    float FallFullDamageSpeed = 1800.0f;
    /** 首次身体撞地可造成的最大伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|跌落", meta = (DisplayName = "跌落满额伤害", ClampMin = "0"))
    float FallMaximumDamage = 500.0f;

    /** 本目标接受冲击保持时间的最大秒数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命", meta = (DisplayName = "冲击保持时间上限", ClampMin = "0"))
    float MaximumImpactHoldSeconds = 10.0f;

    /** 每次进入倒地状态的独立生命 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命", meta = (DisplayName = "倒地生命", ClampMin = "1"))
    float DownedHealth = 200.0f;

    /** 致死或击倒一击的最低水平后推速度 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|击倒", meta = (DisplayName = "致死最低后推速度", ClampMin = "0", ForceUnits = "cm/s"))
    float LethalKnockbackSpeed = 500.0f;

    /** 致死或击倒一击的最低向上抬升速度 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|击倒", meta = (DisplayName = "致死最低抬升速度", ClampMin = "0", ForceUnits = "cm/s"))
    float LethalKnockbackLift = 180.0f;

    /** 进入倒地状态时触发物理砸地的最短保持秒数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|击倒", meta = (DisplayName = "倒地物理保持时长", ClampMin = "0", ForceUnits = "s"))
    float DownedImpactHoldSeconds = 0.7f;

    /** 致死或击倒布娃娃时上半身额外仰翻速度比例 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|击倒", meta = (DisplayName = "布娃娃上半身仰翻比例", ClampMin = "0", ClampMax = "2"))
    float LethalUpperBodyImpulseScale = 0.45f;

    /** 救援开始与维持的最大距离 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|救援", meta = (DisplayName = "救援距离", ClampMin = "1", ToolTip = "双方固定脚底基准的最大距离"))
    float RescueDistance = 100.0f;

    /** 被救者接受后完成救援需要的时间 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|救援", meta = (DisplayName = "救援时长", ClampMin = "0.01", ToolTip = "由被救者控制端计时"))
    float RescueDuration = 3.0f;

    /** 救援成功后恢复的正常生命 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|救援", meta = (DisplayName = "救援恢复生命", ClampMin = "1", ToolTip = "不超过角色最大生命"))
    float RescueHealth = 75.0f;

    /** 角色移动参数与碰撞配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "移动配置"))
    FBBBCharacterLocomotionConfig Locomotion;


    /** 角色瞄准动画配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "瞄准动画"))
    FBBBAimAnimationConfig AimAnimation;

    /** 角色相机构图与回正配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "相机"))
    FBBBCharacterCameraConfig Camera;

    /** 角色动画事实识别配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "动画"))
    FBBBCharacterAnimationConfig Animation;

    /** 角色背包与前序快捷槽位配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品"))
    FBBBCharacterItemConfig Item;
    /** 基础身体与无装备回退资源 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "外观配置"))
    FBBBCharacterAppearanceConfig Appearance;

    /** 角色四类物品注册与容器配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备"))
    FBBBCharacterInteractionConfig Interaction;

    /** 角色网络状态同步配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "网络配置"))
    FBBBCharacterNetworkConfig Network;
};
