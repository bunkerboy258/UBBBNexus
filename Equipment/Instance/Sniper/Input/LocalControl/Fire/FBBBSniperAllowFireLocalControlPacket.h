#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionInputState.h"

/** 狙击枪动画开火限制输入 */
struct FBBBSniperAllowFireLocalControlPacket final
{
    /** @return 请求有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 可以写入本帧请求 */
    bool CanApply() const
    {
        return true;
    }

    /** @param State	狙击枪动作输入 @return 无 */
    void Apply(FBBBSniperActionInputState &State) const
    {
        State.bAllowFireRequested = true;
    }
};
