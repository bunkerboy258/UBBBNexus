#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/AnimationSystem/DomainData/States/BBBMeleeAnimationState.h"
/** 近战Animation领域状态的唯一持有者 */
struct FBBBMeleeAnimationDomainState final
{
    /** @return Animation领域的Animation状态 */
    const FBBBMeleeAnimationState &ReadMeleeAnimationState() const
    {
        return AnimationState;
    }
private:
    friend class FBBBMeleeAnimationProcessor;
    /** Animation状态 */
    FBBBMeleeAnimationState AnimationState;
};
