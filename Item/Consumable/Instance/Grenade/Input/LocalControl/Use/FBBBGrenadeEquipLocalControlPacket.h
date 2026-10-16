#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableEquipLocalControlPacket.h"
#include "CoreMinimal.h"

/** 消耗品建立持有输入 */
struct FBBBGrenadeEquipLocalControlPacket final : public FBBBConsumableEquipLocalControlPacket<FBBBGrenadeEquipLocalControlPacket>
{
    /** @return 输入格式是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	解析上下文 @return 是否拥有本机因果权限 */
    bool CanApply(bool bIsMirror, const FBBBGrenadeUseState &State) const
    {
        return !bIsMirror;
    }

    /** @param Context	解析上下文 @return 无 */
    void Apply(FBBBGrenadeUseInputState &Inputs, FBBBGrenadeUseState &State) const
    {
        Inputs.bEquip = true;
    }
};
