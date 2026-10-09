#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionInputState.h"

/** 霰弹枪装入弹药输入 */
struct FBBBShotgunLoadAmmoLocalControlPacket final
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
     * @param State	霰弹枪动作输入状态
     * @return 无
     */
    void Apply(FBBBShotgunActionInputState &State) const
    {
        State.bLoadAmmoRequested = true;
    }
};
