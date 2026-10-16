#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionInputState.h"

/** 左轮Reload输入效果 */
struct FBBBRevolverSecondaryLocalControlPacket final : public FBBBEquipmentSecondaryLocalControlPacket<FBBBRevolverSecondaryLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBRevolverActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBRevolverActionInputState &State) const
    {
        State.bReloadRequested = true;
    }
};
