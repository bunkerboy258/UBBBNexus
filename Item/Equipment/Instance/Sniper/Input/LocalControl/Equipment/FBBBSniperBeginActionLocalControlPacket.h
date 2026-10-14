#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginActionLocalControlPacket.h"

/** 本类装备不支持的公共操作 明确拒绝且不贡献状态 */
struct FBBBSniperBeginActionLocalControlPacket final : public FBBBEquipmentBeginActionLocalControlPacket<FBBBSniperBeginActionLocalControlPacket>
{
    /** @return 请求格式有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @return 本类不支持此操作 */
    bool CanApply() const
    {
        return false;
    }
    /** @param State 不修改的本类状态 @return 无 */
    template<typename TState>
    void Apply(TState &State) const
    {
    }
};
