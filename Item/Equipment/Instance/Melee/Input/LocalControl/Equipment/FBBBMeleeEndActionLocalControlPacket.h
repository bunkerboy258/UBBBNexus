#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEndActionLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionInputState.h"

/** 近战EndAction通知输入 */
struct FBBBMeleeEndActionLocalControlPacket final : public FBBBEquipmentEndActionLocalControlPacket<FBBBMeleeEndActionLocalControlPacket>
{
    /** @return 通知实例标识有效 */
    bool IsValid() const
    {
        return !Tokens.IsEmpty() && !Tokens.ContainsByPredicate([](int32 Value)
        {
            return Value < 0;
        });
    }

    /** @return 可以写入当前动作请求 */
    bool CanApply() const
    {
        return true;
    }

    /** @param State	近战动作输入 @return 无 */
    void Apply(FBBBMeleeActionInputState &State) const
    {
        State.EndActions.Append(Tokens);
    }
};
