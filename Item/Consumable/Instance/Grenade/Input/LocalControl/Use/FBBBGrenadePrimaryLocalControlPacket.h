#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePrimaryLocalControlPacket.h"
#include "CoreMinimal.h"

/** 消耗品主行为输入 */
struct FBBBGrenadePrimaryLocalControlPacket final : public FBBBConsumablePrimaryLocalControlPacket<FBBBGrenadePrimaryLocalControlPacket>
{
    /** @return 输入格式是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	解析上下文 @return 是否拥有本机因果权限 */
    bool CanApply(bool bIsMirror, const FBBBGrenadeUseState &State) const
    {
        return !bIsMirror && !State.bHide;
    }

    /** @param Context	解析上下文 @return 无 */
    void Apply(FBBBGrenadeUseInputState &Inputs, FBBBGrenadeUseState &State) const
    {
        Inputs.bPrimary = true;
    }
};
