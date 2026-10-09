#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionInputState.h"

/** 霰弹枪动画开火限制输入 */
struct FBBBShotgunAllowFireLocalControlPacket final
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

    /** @param State	霰弹枪动作输入 @return 无 */
    void Apply(FBBBShotgunActionInputState &State) const
    {
        State.bAllowFireRequested = true;
    }
};
