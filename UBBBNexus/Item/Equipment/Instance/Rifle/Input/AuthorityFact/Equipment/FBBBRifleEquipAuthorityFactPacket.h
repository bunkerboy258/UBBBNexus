#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionInputState.h"

/** 步枪Equip输入效果 */
struct FBBBRifleEquipAuthorityFactPacket final : public FBBBEquipmentEquipAuthorityFactPacket<FBBBRifleEquipAuthorityFactPacket>
{
    /** @return 输入数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 输入可写入本帧状态 */
    bool CanApply(const FBBBRifleActionState &State) const
    {
        return true;
    }

    /** @param State	本类装备的动作输入状态 @return 无 */
    void Apply(FBBBRifleActionInputState &State) const
    {
        State.bEquipRequested = true;
    }
};
