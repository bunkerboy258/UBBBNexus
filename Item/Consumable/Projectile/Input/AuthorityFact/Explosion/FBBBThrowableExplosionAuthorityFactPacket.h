#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableExplosionFact.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/DomainData/States/BBBThrowableExplosionState.h"
/** UE 复制接收的投掷物终态 */
struct FBBBThrowableExplosionAuthorityFactPacket final
{
    FBBBThrowableExplosionFact Fact;
    /** @return 事实结构是否有效 */
    bool IsValid() const { return Fact.bExploded && !Fact.Position.ContainsNaN() && FMath::IsFinite(Fact.Time); }
    /** @param State	爆炸状态 @return 是否未接收终态 */
    bool CanApply(const FBBBThrowableExplosionState& State) const { return !State.Fact.bExploded; }
    /** @param State	爆炸状态 @return 无 */
    void Apply(FBBBThrowableExplosionState& State) const { State.Fact = Fact; }
};
