#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionInputState.h"

/** 狙击枪Fire输入效果 */
struct FBBBSniperPrimaryLocalControlPacket final : public FBBBEquipmentPrimaryLocalControlPacket<FBBBSniperPrimaryLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return PressCount >= 0;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBSniperActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBSniperActionInputState &State) const
    {
        State.bPrimaryRequested = true;
        State.PrimaryPressCount += PressCount;
    }
};
