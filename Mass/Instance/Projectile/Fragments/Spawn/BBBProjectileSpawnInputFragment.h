#pragma once

#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"
#include "BBBProjectileSpawnInputFragment.generated.h"

/** 子弹出生输入覆盖槽 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectileSpawnInputFragment final : public FMassFragment
{
    GENERATED_BODY()

    TBBBMassInputSlot<FBBBProjectileSpawnLocalControlPacket> Spawn;

    template<typename TPacket>
    TBBBMassInputSlot<TPacket>& GetSlot()
    {
        static_assert(std::is_same_v<TPacket, FBBBProjectileSpawnLocalControlPacket>);
        return Spawn;
    }
};
