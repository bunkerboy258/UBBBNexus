#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionInputState.h"

/** 左轮Equip输入效果 */
struct FBBBRevolverEquipLocalControlPacket final : public FBBBEquipmentEquipLocalControlPacket<FBBBRevolverEquipLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBRevolverActionState &State) const
    {
        return true;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBRevolverActionInputState &State) const
    {
        State.bEquipRequested = true;
    }
};
