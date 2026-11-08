#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"

/** 本类装备不支持的公共操作 明确拒绝且不贡献状态 */
struct FBBBMeleeAllowFireLocalControlPacket final : public FBBBEquipmentAllowFireLocalControlPacket<FBBBMeleeAllowFireLocalControlPacket>
{
    /** @return 请求格式有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @return 本类不支持此操作 */
    bool CanApply(const FBBBMeleeActionState &State) const
    {
        return false;
    }
    /** @param State 不修改的本类状态 @return 无 */
    template<typename TState>
    void Apply(TState &State) const
    {
    }
};
