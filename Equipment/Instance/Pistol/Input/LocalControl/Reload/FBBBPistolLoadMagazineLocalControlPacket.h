#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionInputState.h"

/** 手枪装入弹匣输入 */
struct FBBBPistolLoadMagazineLocalControlPacket final
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
     * @param State	手枪动作输入状态
     * @return 无
     */
    void Apply(FBBBPistolActionInputState &State) const
    {
        State.bLoadMagazineRequested = true;
    }
};
