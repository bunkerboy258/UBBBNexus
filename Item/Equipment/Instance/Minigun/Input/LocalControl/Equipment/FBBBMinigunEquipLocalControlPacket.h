#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionInputState.h"

/** 转管机枪Equip输入效果 */
struct FBBBMinigunEquipLocalControlPacket final : public FBBBEquipmentEquipLocalControlPacket<FBBBMinigunEquipLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBMinigunActionState &State) const
    {
        return true;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBMinigunActionInputState &State) const
    {
        State.bEquipRequested = true;
    }
};
