#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionState.h"

/** 网格显示请求 不改变玩法动作 */
struct FBBBRevolverShowLocalControlPacket final : public FBBBEquipmentShowLocalControlPacket<FBBBRevolverShowLocalControlPacket>
{
    /** @return 请求格式有效 */
    bool IsValid() const { return true; }
    /** @param State 当前事实 @return 允许设置网格状态 */
    bool CanApply(const FBBBRevolverActionState &State) const { return true; }
    /** @param State 本装备网格状态 @return 无 */
    void Apply(FBBBRevolverActionState &State) const { State.bHide = false; }
};
