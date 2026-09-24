#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/DomainData/States/BBBRifleAnimationState.h"

/** 步枪Animation领域状态的唯一持有者 */
struct FBBBRifleAnimationDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBRifleAnimationState &ReadRifleAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBRiflePoseProcessor;
    friend class FBBBRifleAnimationProcessor;
    /** 本领域处理器维护的状态 */
    FBBBRifleAnimationState AnimationState;
};
