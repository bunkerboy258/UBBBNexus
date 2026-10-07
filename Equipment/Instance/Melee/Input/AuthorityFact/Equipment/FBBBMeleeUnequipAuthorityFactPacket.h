#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionInputState.h"

/** 近战Unequip输入效果 */
struct FBBBMeleeUnequipAuthorityFactPacket final
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply() const
    {
        return true;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBMeleeActionInputState &State) const
    {
        State.bUnequip = true;
    }
};
