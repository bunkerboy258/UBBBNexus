#pragma once
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBMeleeDefinition.generated.h"
class UAnimMontage;
/** 单件近战装备的静态配置 */
UCLASS(BlueprintType, meta = (DisplayName = "近战装备配置"))
class ABBB_EVAC_API UBBBMeleeDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()
public:
    /** @return 无 为近战配置建立明确类型 */
    UBBBMeleeDefinition()
    {
        EquipmentType = EBBBEquipmentType::Melee;
    }

    /** 角色播放的单次攻击蒙太奇 使用 UpperBody 槽位 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|动画", meta = (DisplayName = "攻击蒙太奇"))
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;
    /** 从武器握持端开始的伤害扫掠插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中", meta = (DisplayName = "扫掠起点插槽"))
    FName TraceStartSocket = TEXT("AttackBase");
    /** 武器打击端的伤害扫掠插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中", meta = (DisplayName = "扫掠终点插槽"))
    FName TraceEndSocket = TEXT("AttackTip");
    /** 每轮攻击对单个目标造成的伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中", meta = (ClampMin = "0.0", DisplayName = "攻击伤害"))
    float Damage = 25.0f;
    /** 命中耐久部位时混合的伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中", meta = (ClampMin = "0.0", DisplayName = "耐久伤害"))
    float DurableDamage = 25.0f;
    /** 覆盖武器运动轨迹的球扫掠半径 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中", meta = (ClampMin = "0.1", DisplayName = "扫掠半径"))
    float TraceRadius = 5.0f;
    /** 相邻攻击开始时间的最小间隔 不替代动画结束通知 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|攻击", meta = (ClampMin = "0.01", DisplayName = "攻击间隔"))
    float AttackInterval = 0.9f;
    /** 用于世界遮挡与普通 Actor 受击的碰撞通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中", meta = (DisplayName = "命中通道"))
    TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Visibility;
};
