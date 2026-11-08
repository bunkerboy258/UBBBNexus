#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionInputState.h"

/** 霰弹枪动画开火限制输入 */
struct FBBBShotgunBlockFireLocalControlPacket final : public FBBBEquipmentBlockFireLocalControlPacket<FBBBShotgunBlockFireLocalControlPacket>
{
    /** @return 请求有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 可以写入本帧请求 */
    bool CanApply(const FBBBShotgunActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	霰弹枪动作输入 @return 无 */
    void Apply(FBBBShotgunActionInputState &State) const
    {
        State.bBlockFireRequested = true;
    }
};
