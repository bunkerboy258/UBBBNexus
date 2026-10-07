#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionInputState.h"
/** 近战Action领域状态的唯一持有者 */
struct FBBBMeleeActionDomainState final
{
    /** @return Action领域的Action状态 */
    const FBBBMeleeActionState &ReadMeleeActionState() const
    {
        return ActionState;
    }
    /** @return Action领域的Input状态 */
    const FBBBMeleeActionInputState &ReadMeleeActionInputState() const
    {
        return InputState;
    }
private:
    friend class FBBBMeleeActionProcessor;
    friend class FBBBMeleeParseProcessor;
    friend class FBBBMeleeContactProcessor;
    /** Action状态 */
    FBBBMeleeActionState ActionState;
    /** Input状态 */
    FBBBMeleeActionInputState InputState;
};
