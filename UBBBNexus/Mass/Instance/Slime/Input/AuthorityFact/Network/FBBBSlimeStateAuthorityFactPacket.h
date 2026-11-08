#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFragment.h"

struct FBBBSlimeInputFragment;

/** 接收当前粘液结果 不重演飞行碰撞 */
struct FBBBSlimeStateAuthorityFactPacket final
{
    using FInputFragment = FBBBSlimeInputFragment;
    FBBBSlimeFact Fact;
    /** @return 当前事实是否完整 */
    bool IsValid() const { return Fact.IsValid(); }
    /** @param Current 当前实例 @return 身份与版本是否允许接收 */
    bool CanApply(const FBBBSlimeFragment& Current) const
    {
        return (!Current.Fact.InstanceId.IsValid() || Current.Fact.InstanceId == Fact.InstanceId)
            && Fact.Revision > Current.Fact.Revision;
    }
    /** @param Current 当前实例 @return 只复制当前结果 */
    void Apply(FBBBSlimeFragment& Current) const { Current.Fact = Fact; }
};
