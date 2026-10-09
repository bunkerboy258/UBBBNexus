#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionInputState.h"

/** 手枪Fire输入效果 */
struct FBBBPistolFireLocalControlPacket final
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
    void Apply(FBBBPistolActionInputState &State) const
    {
        State.bPrimaryRequested = true;
    }
};
