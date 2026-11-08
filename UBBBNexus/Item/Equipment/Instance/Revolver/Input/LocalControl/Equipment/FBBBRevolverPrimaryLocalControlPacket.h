#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionInputState.h"

/** 左轮Fire输入效果 */
struct FBBBRevolverPrimaryLocalControlPacket final : public FBBBEquipmentPrimaryLocalControlPacket<FBBBRevolverPrimaryLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return PressCount >= 0;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBRevolverActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBRevolverActionInputState &State) const
    {
        State.bPrimaryRequested = true;
        State.PrimaryPressCount += PressCount;
    }
};
