#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionInputState.h"

/** 冲锋枪动画开火限制输入 */
struct FBBBSMGBlockFireLocalControlPacket final : public FBBBEquipmentBlockFireLocalControlPacket<FBBBSMGBlockFireLocalControlPacket>
{
    /** @return 请求有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 可以写入本帧请求 */
    bool CanApply(const FBBBSMGActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	冲锋枪动作输入 @return 无 */
    void Apply(FBBBSMGActionInputState &State) const
    {
        State.bBlockFireRequested = true;
    }
};
