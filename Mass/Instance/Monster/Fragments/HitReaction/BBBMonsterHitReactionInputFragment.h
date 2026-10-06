#pragma once

#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/HitReaction/FBBBMonsterHitReactionLocalControlPacket.h"
#include "BBBMonsterHitReactionInputFragment.generated.h"

/** 最近命中覆盖槽 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterHitReactionInputFragment final : public FMassFragment
{
    GENERATED_BODY()

    TBBBMassInputSlot<FBBBMonsterHitReactionLocalControlPacket> Hit;

    /**
     * @return 对应命中表现覆盖槽
     */
    template<typename TPacket>
    TBBBMassInputSlot<TPacket>& GetSlot()
    {
        static_assert(std::is_same_v<TPacket, FBBBMonsterHitReactionLocalControlPacket>);
        return Hit;
    }
};
