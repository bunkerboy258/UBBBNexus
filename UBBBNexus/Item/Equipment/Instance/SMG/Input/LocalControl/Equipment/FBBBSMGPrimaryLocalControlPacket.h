#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionInputState.h"

/** 冲锋枪Fire输入效果 */
struct FBBBSMGPrimaryLocalControlPacket final : public FBBBEquipmentPrimaryLocalControlPacket<FBBBSMGPrimaryLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return PressCount >= 0;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBSMGActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBSMGActionInputState &State) const
    {
        State.bPrimaryRequested = true;
        State.PrimaryPressCount += PressCount;
    }
};
