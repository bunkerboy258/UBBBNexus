#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionInputState.h"

/** 转管机枪结束换弹输入 */
struct FBBBMinigunInterruptReloadLocalControlPacket final
{
    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入是否可写入本帧状态 */
    bool CanApply() const
    {
        return true;
    }

    /**
     * 将请求写入动作领域状态
     * @param State	转管机枪动作输入状态
     * @return 无
     */
    void Apply(FBBBMinigunActionInputState &State) const
    {
        State.bInterruptReloadRequested = true;
    }
};
