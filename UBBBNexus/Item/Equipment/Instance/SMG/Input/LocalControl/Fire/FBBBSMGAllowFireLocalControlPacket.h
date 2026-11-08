#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionInputState.h"

/** 冲锋枪动画开火限制输入 */
struct FBBBSMGAllowFireLocalControlPacket final : public FBBBEquipmentAllowFireLocalControlPacket<FBBBSMGAllowFireLocalControlPacket>
{
    /** @return 请求有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 可以写入本帧请求 */
    bool CanApply(const FBBBSMGActionState &State) const
    {
        return true;
    }

    /** @param State	冲锋枪动作输入 @return 无 */
    void Apply(FBBBSMGActionInputState &State) const
    {
        State.bAllowFireRequested = true;
    }
};
