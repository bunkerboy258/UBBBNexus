#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
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

    /** 角色装备本件近战武器时播放的蒙太奇 使用 UpperBody 槽位覆盖上一件装备的动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|动画|角色", meta = (DisplayName = "角色装备蒙太奇", ToolTip = "装备时提交角色 UpperBody 槽位 用于替换上一件装备占用该槽位的动作"))
    TObjectPtr<UAnimMontage> CharacterEquipMontage = nullptr;

    /** 角色播放的单次攻击蒙太奇 使用 UpperBody 槽位 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|动画|角色", meta = (DisplayName = "角色攻击蒙太奇", ToolTip = "每次攻击提交角色 UpperBody 槽位 攻击和命中窗口由本动作的逻辑通知驱动"))
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;
    /** 从武器握持端开始的伤害扫掠插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中检测", meta = (DisplayName = "扫掠起点插槽", ToolTip = "本武器网格上的握持端插槽 与终点插槽共同确定本帧和上一帧的扫掠轨迹"))
    FName TraceStartSocket = TEXT("AttackBase");
    /** 武器打击端的伤害扫掠插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中检测", meta = (DisplayName = "扫掠终点插槽", ToolTip = "本武器网格上的打击端插槽 与起点插槽共同确定攻击覆盖长度"))
    FName TraceEndSocket = TEXT("AttackTip");
    /** 每轮攻击对单个目标造成的伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|伤害", meta = (ClampMin = "0.0", DisplayName = "普通伤害 (生命值)", ToolTip = "每次攻击对同一目标只计伤一次 越大伤害越高 Mass 部位按普通伤害和耐久伤害混合 普通 Actor 只用此值"))
    float Damage = 25.0f;
    /** 命中耐久部位时混合的伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|伤害", meta = (ClampMin = "0.0", DisplayName = "耐久伤害 (生命值)", ToolTip = "用于 Mass 部位耐久混合 有效伤害为普通伤害*(1-耐久比例)+耐久伤害*耐久比例 不是额外叠加 普通 Actor 不使用"))
    float DurableDamage = 25.0f;
    /** 覆盖武器运动轨迹的球扫掠半径 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中检测", meta = (ClampMin = "0.1", DisplayName = "扫掠半径 (厘米)", ToolTip = "攻击窗口内球形扫掠的半径 越大越容易覆盖目标 不是武器长度 同时影响轨迹采样间距"))
    float TraceRadius = 5.0f;
    /** 相邻攻击开始时间的最小间隔 不替代动画结束通知 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|攻击", meta = (ClampMin = "0.01", DisplayName = "攻击间隔 (秒)", ToolTip = "相邻攻击开始时间的最小间隔 越小可越快再次攻击 上次攻击仍未结束时也不能开始下一次"))
    float AttackInterval = 0.9f;
    /** 用于世界遮挡与普通 Actor 受击的碰撞通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|近战|命中检测", meta = (DisplayName = "世界碰撞通道", ToolTip = "用于世界遮挡及普通 Actor 的球形扫掠 Mass 小怪另走实体查询 不由此通道选择"))
    TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Visibility;
};
