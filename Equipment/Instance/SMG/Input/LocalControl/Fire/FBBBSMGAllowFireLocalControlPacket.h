#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionInputState.h"

/** 冲锋枪动画开火限制输入 */
struct FBBBSMGAllowFireLocalControlPacket final
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

    /** @param State	冲锋枪动作输入 @return 无 */
    void Apply(FBBBSMGActionInputState &State) const
    {
        State.bAllowFireRequested = true;
    }
};
