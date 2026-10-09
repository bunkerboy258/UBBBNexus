#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionInputState.h"

/** 狙击枪Reload输入效果 */
struct FBBBSniperReloadLocalControlPacket final
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
    void Apply(FBBBSniperActionInputState &State) const
    {
        State.bReloadRequested = true;
    }
};
