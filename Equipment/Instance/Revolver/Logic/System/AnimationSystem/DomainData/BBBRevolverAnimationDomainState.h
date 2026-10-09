#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/DomainData/States/BBBRevolverAnimationState.h"

/** 左轮Animation领域状态的唯一持有者 */
struct FBBBRevolverAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBRevolverAnimationState &ReadRevolverAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBRevolverPoseProcessor;
    friend class FBBBRevolverAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBRevolverAnimationState AnimationState;
};
