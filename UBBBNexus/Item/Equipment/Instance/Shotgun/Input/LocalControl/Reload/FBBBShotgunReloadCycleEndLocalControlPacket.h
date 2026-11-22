#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionState.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionInputState.h"

/** 记录单发装填自然结束或收手动作退出 */
struct FBBBShotgunReloadCycleEndLocalControlPacket final
{
    /** 本通知来自收手动作 而不是一发装入动作 */
    bool bFinishing = false;

    /** @return 两种结束阶段均有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 请求允许写入本帧状态 */
    bool CanApply(const FBBBShotgunActionState &State) const
    {
        return true;
    }

    /**
     * 提交对应阶段的结束事实 不执行动作
     * @param State	动作输入状态
     * @return 无
     */
    void Apply(FBBBShotgunActionInputState &State) const
    {
        State.bReloadCycleEnded = !bFinishing;
        State.bReloadFinishEnded = bFinishing;
    }
};
