#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/States/BBBLMGActionInputState.h"

/** 轻机枪动画开火限制输入 */
struct FBBBLMGAllowFireLocalControlPacket final
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

    /** @param State	轻机枪动作输入 @return 无 */
    void Apply(FBBBLMGActionInputState &State) const
    {
        State.bAllowFireRequested = true;
    }
};
