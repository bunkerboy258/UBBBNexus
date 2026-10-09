#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/DomainData/States/BBBSniperAnimationState.h"

/** 狙击枪Animation领域状态的唯一持有者 */
struct FBBBSniperAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBSniperAnimationState &ReadSniperAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBSniperPoseProcessor;
    friend class FBBBSniperAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBSniperAnimationState AnimationState;
};
