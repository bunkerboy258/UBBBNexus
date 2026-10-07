#pragma once
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBMeleeAnimInstance.generated.h"
/** 近战装备的动画事实快照 */
UCLASS(Transient, Blueprintable, meta = (DisplayName = "近战装备动画实例"))
class ABBB_EVAC_API UBBBMeleeAnimInstance final : public UBBBEquipmentAnimInstance
{
    GENERATED_BODY()
public:
    /** @return 当前攻击序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|近战", meta = (DisplayName = "攻击序号", BlueprintThreadSafe))
    int32 GetAttackSequence() const;
    /** @return 是否正在攻击 */
    UFUNCTION(BlueprintPure, Category = "BBB|近战", meta = (DisplayName = "正在攻击", BlueprintThreadSafe))
    bool IsAttacking() const;
private:
    friend class FBBBMeleeAnimationProcessor;
    /** 已发布的攻击序号 */
    int32 AttackSequence = 0;
    /** 已发布的攻击状态 */
    bool bAttacking = false;
};
