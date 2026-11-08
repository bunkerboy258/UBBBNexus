#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionState.h"

/** 网格隐藏请求 不改变玩法动作 */
struct FBBBSniperHideLocalControlPacket final : public FBBBEquipmentHideLocalControlPacket<FBBBSniperHideLocalControlPacket>
{
    /** @return 请求格式有效 */
    bool IsValid() const { return true; }
    /** @param State 当前事实 @return 允许设置网格状态 */
    bool CanApply(const FBBBSniperActionState &State) const { return true; }
    /** @param State 本装备网格状态 @return 无 */
    void Apply(FBBBSniperActionState &State) const { State.bHide = true; }
};
