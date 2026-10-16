#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
/** 手持网格Show请求 不改变使用生命周期 */
struct FBBBGrenadeShowLocalControlPacket final : public FBBBConsumableShowLocalControlPacket<FBBBGrenadeShowLocalControlPacket>
{
    bool IsValid() const { return true; }
    bool CanApply(bool bIsMirror, const FBBBGrenadeUseState &State) const { return !bIsMirror; }
    void Apply(FBBBGrenadeUseInputState &Input, FBBBGrenadeUseState &State) const { State.bHide = false; }
};
