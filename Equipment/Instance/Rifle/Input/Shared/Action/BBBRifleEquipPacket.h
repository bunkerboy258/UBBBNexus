#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionInputState.h"

/** 步枪装备输入 */
struct FBBBRifleEquipPacket final
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
     * @param State	步枪动作输入状态
     * @return 无
     */
    void Apply(FBBBRifleActionInputState &State) const
    {
        State.bEquipRequested = true;
    }
};
