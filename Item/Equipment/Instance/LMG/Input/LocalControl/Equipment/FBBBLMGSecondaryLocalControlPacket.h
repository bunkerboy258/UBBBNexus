#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/States/BBBLMGActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/States/BBBLMGActionInputState.h"

/** 轻机枪Reload输入效果 */
struct FBBBLMGSecondaryLocalControlPacket final : public FBBBEquipmentSecondaryLocalControlPacket<FBBBLMGSecondaryLocalControlPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBLMGActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBLMGActionInputState &State) const
    {
        State.bReloadRequested = true;
    }
};
