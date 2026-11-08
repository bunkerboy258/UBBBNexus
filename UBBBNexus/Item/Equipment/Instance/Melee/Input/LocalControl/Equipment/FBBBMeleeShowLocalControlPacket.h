#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/States/BBBMeleeActionState.h"

/** 网格显示请求 不改变玩法动作 */
struct FBBBMeleeShowLocalControlPacket final : public FBBBEquipmentShowLocalControlPacket<FBBBMeleeShowLocalControlPacket>
{
    /** @return 请求格式有效 */
    bool IsValid() const { return true; }
    /** @param State 当前事实 @return 允许设置网格状态 */
    bool CanApply(const FBBBMeleeActionState &State) const { return true; }
    /** @param State 本装备网格状态 @return 无 */
    void Apply(FBBBMeleeActionState &State) const { State.bHide = false; }
};
