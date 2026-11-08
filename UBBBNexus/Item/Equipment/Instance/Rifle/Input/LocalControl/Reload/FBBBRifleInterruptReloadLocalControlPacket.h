#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionInputState.h"

/** 步枪结束换弹输入 */
struct FBBBRifleInterruptReloadLocalControlPacket final : public FBBBEquipmentInterruptReloadLocalControlPacket<FBBBRifleInterruptReloadLocalControlPacket>
{
    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入是否可写入本帧状态 */
    bool CanApply(const FBBBRifleActionState &State) const
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
        State.bInterruptReloadRequested = true;
    }
};
