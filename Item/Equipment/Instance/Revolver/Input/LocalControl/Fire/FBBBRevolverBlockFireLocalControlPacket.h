#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionInputState.h"

/** 左轮动画开火限制输入 */
struct FBBBRevolverBlockFireLocalControlPacket final : public FBBBEquipmentBlockFireLocalControlPacket<FBBBRevolverBlockFireLocalControlPacket>
{
    /** @return 请求有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 可以写入本帧请求 */
    bool CanApply() const
    {
        return true;
    }

    /** @param State	左轮动作输入 @return 无 */
    void Apply(FBBBRevolverActionInputState &State) const
    {
        State.bBlockFireRequested = true;
    }
};
