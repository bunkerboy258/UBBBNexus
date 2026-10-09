#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/AnimationSystem/DomainData/States/BBBLMGAnimationState.h"

/** 轻机枪Animation领域状态的唯一持有者 */
struct FBBBLMGAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBLMGAnimationState &ReadLMGAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBLMGPoseProcessor;
    friend class FBBBLMGAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBLMGAnimationState AnimationState;
};
