#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionInputState.h"

/** 手枪Reload输入效果 */
struct FBBBPistolSecondaryLocalControlPacket final : public FBBBEquipmentSecondaryLocalControlPacket<FBBBPistolSecondaryLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBPistolActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBPistolActionInputState &State) const
    {
        State.bReloadRequested = true;
    }
};
