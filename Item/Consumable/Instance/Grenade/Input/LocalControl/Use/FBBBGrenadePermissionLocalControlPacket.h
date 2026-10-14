#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePermissionLocalControlPacket.h"
#include "CoreMinimal.h"

/** 持有者发布的通用使用许可 */
struct FBBBGrenadePermissionLocalControlPacket final : public FBBBConsumablePermissionLocalControlPacket<FBBBGrenadePermissionLocalControlPacket>
{

    /** @return 输入是否包含许可 */
    bool IsValid() const
    {
        return !Values.IsEmpty();
    }

    /** @param Context	解析上下文 @return 是否拥有因果权限 */
    bool CanApply(bool bIsMirror) const
    {
        return !bIsMirror;
    }

    /** @param Context	解析上下文 @return 无 */
    void Apply(FBBBGrenadeUseInputState &Inputs, FBBBGrenadeUseState &State) const
    {
        Inputs.bHasPermission = true;
        Inputs.bAllowed = Values.Last();
    }
};
