#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/DomainData/States/BBBMeleeInputState.h"
/** 近战Parse领域状态的唯一持有者 */
struct FBBBMeleeParseDomainState final
{
    /** @return Parse领域的Input状态 */
    const FBBBMeleeInputState &ReadMeleeInputState() const
    {
        return InputState;
    }
private:
    friend class FBBBMeleeParseProcessor;
    /** Input状态 */
    FBBBMeleeInputState InputState;
};
