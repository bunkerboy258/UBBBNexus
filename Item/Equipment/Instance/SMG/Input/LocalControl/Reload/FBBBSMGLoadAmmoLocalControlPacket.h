#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionInputState.h"

/** 冲锋枪装入弹匣输入 */
struct FBBBSMGLoadAmmoLocalControlPacket final : public FBBBEquipmentLoadAmmoLocalControlPacket<FBBBSMGLoadAmmoLocalControlPacket>
{
    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入是否可写入本帧状态 */
    bool CanApply(const FBBBSMGActionState &State) const
    {
        return true;
    }

    /**
     * 将请求写入动作领域状态
     * @param State	冲锋枪动作输入状态
     * @return 无
     */
    void Apply(FBBBSMGActionInputState &State) const
    {
        State.bLoadMagazineRequested = true;
    }
};
