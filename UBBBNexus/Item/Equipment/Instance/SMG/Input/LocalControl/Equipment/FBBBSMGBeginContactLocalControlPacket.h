#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"

/** 本类装备不支持的公共操作 明确拒绝且不贡献状态 */
struct FBBBSMGBeginContactLocalControlPacket final : public FBBBEquipmentBeginContactLocalControlPacket<FBBBSMGBeginContactLocalControlPacket>
{
    /** @return 请求格式有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @return 本类不支持此操作 */
    bool CanApply(const FBBBSMGActionState &State) const
    {
        return false;
    }
    /** @param State 不修改的本类状态 @return 无 */
    template<typename TState>
    void Apply(TState &State) const
    {
    }
};
