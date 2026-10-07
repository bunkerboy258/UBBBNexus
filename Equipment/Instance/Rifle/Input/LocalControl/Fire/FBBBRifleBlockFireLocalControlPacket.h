#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionInputState.h"

/** 步枪动画开火限制输入 */
struct FBBBRifleBlockFireLocalControlPacket final
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

    /** @param State	步枪动作输入 @return 无 */
    void Apply(FBBBRifleActionInputState &State) const
    {
        State.bBlockFireRequested = true;
    }
};
