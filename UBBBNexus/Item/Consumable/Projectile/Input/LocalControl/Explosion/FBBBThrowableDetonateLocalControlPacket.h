#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/DomainData/States/BBBThrowableExplosionState.h"
/** 物理接触触发的房主引爆请求 */
struct FBBBThrowableDetonateLocalControlPacket final
{
    /** @return 请求结构是否有效 */
    bool IsValid() const { return true; }
    /** @param State	爆炸状态 @return 是否尚未爆炸 */
    bool CanApply(const FBBBThrowableExplosionState& State) const { return !State.Fact.bExploded; }
    /** @param State	爆炸状态 @return 无 */
    void Apply(FBBBThrowableExplosionState& State) const { State.bRequested = true; }
};
