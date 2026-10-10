#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionInputState.h"

/** 一发装填动作与收手已自然完成 */
struct FBBBShotgunReloadCycleEndLocalControlPacket final
{
    /** @return 无字段通知始终有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 请求允许写入本帧状态 */
    bool CanApply() const
    {
        return true;
    }

    /**
     * 提交自然结束事实 不执行动作
     * @param State	动作输入状态
     * @return 无
     */
    void Apply(FBBBShotgunActionInputState &State) const
    {
        State.bReloadCycleEnded = true;
    }
};
