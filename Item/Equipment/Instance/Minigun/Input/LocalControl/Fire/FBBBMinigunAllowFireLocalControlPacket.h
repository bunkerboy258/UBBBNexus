#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionInputState.h"

/** 转管机枪动画开火限制输入 */
struct FBBBMinigunAllowFireLocalControlPacket final : public FBBBEquipmentAllowFireLocalControlPacket<FBBBMinigunAllowFireLocalControlPacket>
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

    /** @param State	转管机枪动作输入 @return 无 */
    void Apply(FBBBMinigunActionInputState &State) const
    {
        State.bAllowFireRequested = true;
    }
};
