#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/States/BBBLMGActionInputState.h"

/** 轻机枪Equip输入效果 */
struct FBBBLMGEquipAuthorityFactPacket final : public FBBBEquipmentEquipAuthorityFactPacket<FBBBLMGEquipAuthorityFactPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply() const
    {
        return true;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBLMGActionInputState &State) const
    {
        State.bEquipRequested = true;
    }
};
