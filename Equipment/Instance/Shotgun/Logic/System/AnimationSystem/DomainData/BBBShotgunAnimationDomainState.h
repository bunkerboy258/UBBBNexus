#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/DomainData/States/BBBShotgunAnimationState.h"

/** 霰弹枪Animation领域状态的唯一持有者 */
struct FBBBShotgunAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBShotgunAnimationState &ReadShotgunAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBShotgunPoseProcessor;
    friend class FBBBShotgunAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBShotgunAnimationState AnimationState;
};
