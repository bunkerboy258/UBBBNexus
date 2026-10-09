#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/DomainData/States/BBBMinigunAnimationState.h"

/** 转管机枪Animation领域状态的唯一持有者 */
struct FBBBMinigunAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBMinigunAnimationState &ReadMinigunAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBMinigunPoseProcessor;
    friend class FBBBMinigunAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBMinigunAnimationState AnimationState;
};
