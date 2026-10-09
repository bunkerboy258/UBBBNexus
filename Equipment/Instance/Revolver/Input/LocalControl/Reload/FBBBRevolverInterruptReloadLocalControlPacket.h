#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionInputState.h"

/** 左轮结束换弹输入 */
struct FBBBRevolverInterruptReloadLocalControlPacket final
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
     * @param State	左轮动作输入状态
     * @return 无
     */
    void Apply(FBBBRevolverActionInputState &State) const
    {
        State.bInterruptReloadRequested = true;
    }
};
