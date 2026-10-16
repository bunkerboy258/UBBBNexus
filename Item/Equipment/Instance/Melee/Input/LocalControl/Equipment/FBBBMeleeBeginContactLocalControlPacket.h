#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentBeginContactLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionInputState.h"

/** 近战BeginContact通知输入 */
struct FBBBMeleeBeginContactLocalControlPacket final : public FBBBEquipmentBeginContactLocalControlPacket<FBBBMeleeBeginContactLocalControlPacket>
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
    bool CanApply(const FBBBMeleeActionState &State) const
    {
        return !State.bHide;
    }

    /** @param State	近战动作输入 @return 无 */
    void Apply(FBBBMeleeActionInputState &State) const
    {
        State.BeginContacts.Append(Tokens);
    }
};
