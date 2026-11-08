#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionInputState.h"

/** 狙击枪Equip输入效果 */
struct FBBBSniperEquipAuthorityFactPacket final : public FBBBEquipmentEquipAuthorityFactPacket<FBBBSniperEquipAuthorityFactPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBSniperActionState &State) const
    {
        return true;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBSniperActionInputState &State) const
    {
        State.bEquipRequested = true;
    }
};
