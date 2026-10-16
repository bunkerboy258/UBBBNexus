#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionInputState.h"

/** 手枪动画开火限制输入 */
struct FBBBPistolAllowFireLocalControlPacket final : public FBBBEquipmentAllowFireLocalControlPacket<FBBBPistolAllowFireLocalControlPacket>
{
    /** @return 请求有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 可以写入本帧请求 */
    bool CanApply(const FBBBPistolActionState &State) const
    {
        return true;
    }

    /** @param State	手枪动作输入 @return 无 */
    void Apply(FBBBPistolActionInputState &State) const
    {
        State.bAllowFireRequested = true;
    }
};
