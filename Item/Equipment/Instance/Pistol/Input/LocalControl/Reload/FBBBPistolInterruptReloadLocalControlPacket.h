#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionInputState.h"

/** 手枪结束换弹输入 */
struct FBBBPistolInterruptReloadLocalControlPacket final : public FBBBEquipmentInterruptReloadLocalControlPacket<FBBBPistolInterruptReloadLocalControlPacket>
{
    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入是否可写入本帧状态 */
    bool CanApply(const FBBBPistolActionState &State) const
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
        State.bInterruptReloadRequested = true;
    }
};
