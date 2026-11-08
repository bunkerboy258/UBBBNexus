#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionInputState.h"

/** 霰弹枪Equip输入效果 */
struct FBBBShotgunEquipLocalControlPacket final : public FBBBEquipmentEquipLocalControlPacket<FBBBShotgunEquipLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBShotgunActionState &State) const
    {
        return true;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBShotgunActionInputState &State) const
    {
        State.bEquipRequested = true;
    }
};
