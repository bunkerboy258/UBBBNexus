#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionState.h"

/** 网格隐藏请求 不改变玩法动作 */
struct FBBBRifleHideLocalControlPacket final : public FBBBEquipmentHideLocalControlPacket<FBBBRifleHideLocalControlPacket>
{
    /** @return 请求格式有效 */
    bool IsValid() const { return true; }
    /** @param State 当前事实 @return 允许设置网格状态 */
    bool CanApply(const FBBBRifleActionState &State) const { return true; }
    /** @param State 本装备网格状态 @return 无 */
    void Apply(FBBBRifleActionState &State) const { State.bHide = true; }
};
