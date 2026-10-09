#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/AnimationSystem/DomainData/States/BBBPistolAnimationState.h"

/** 手枪Animation领域状态的唯一持有者 */
struct FBBBPistolAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBPistolAnimationState &ReadPistolAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBPistolPoseProcessor;
    friend class FBBBPistolAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBPistolAnimationState AnimationState;
};
