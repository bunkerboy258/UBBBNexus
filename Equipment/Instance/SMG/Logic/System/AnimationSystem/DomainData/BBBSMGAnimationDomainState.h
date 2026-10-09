#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/DomainData/States/BBBSMGAnimationState.h"

/** 冲锋枪Animation领域状态的唯一持有者 */
struct FBBBSMGAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBSMGAnimationState &ReadSMGAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBSMGPoseProcessor;
    friend class FBBBSMGAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBSMGAnimationState AnimationState;
};
