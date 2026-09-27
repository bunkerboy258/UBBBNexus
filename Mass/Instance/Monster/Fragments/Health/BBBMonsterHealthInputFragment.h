#pragma once

#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterHealthRemoteMessagePacket.h"
#include "BBBMonsterHealthInputFragment.generated.h"

/** 健康领域输入覆盖槽 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterHealthInputFragment final : public FMassFragment
{
    GENERATED_BODY()

    TBBBMassInputSlot<FBBBMonsterDamageLocalControlPacket> Damage;
    TBBBMassInputSlot<FBBBMonsterHealthRemoteMessagePacket> RemoteHealth;

    template<typename TPacket>
    TBBBMassInputSlot<TPacket>& GetSlot()
    {
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDamageLocalControlPacket>)
        {
            return Damage;
        }
        else
        {
            static_assert(std::is_same_v<TPacket, FBBBMonsterHealthRemoteMessagePacket>);
            return RemoteHealth;
        }
    }
};
