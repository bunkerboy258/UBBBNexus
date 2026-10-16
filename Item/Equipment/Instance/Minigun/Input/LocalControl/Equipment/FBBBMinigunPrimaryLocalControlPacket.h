#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionInputState.h"

/** 转管机枪Fire输入效果 */
struct FBBBMinigunPrimaryLocalControlPacket final : public FBBBEquipmentPrimaryLocalControlPacket<FBBBMinigunPrimaryLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBMinigunActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBMinigunActionInputState &State) const
    {
        State.bPrimaryRequested = true;
    }
};
