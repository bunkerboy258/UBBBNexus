#pragma once

#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Network/FBBBMonsterStateAuthorityFactPacket.h"
#include "BBBMonsterNetworkInputFragment.generated.h"

/** 最新权威事实覆盖槽 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterNetworkInputFragment final : public FMassFragment
{
    GENERATED_BODY()

    TBBBMassInputSlot<FBBBMonsterStateAuthorityFactPacket> State;

    template<typename TPacket>
    TBBBMassInputSlot<TPacket>& GetSlot()
    {
        static_assert(std::is_same_v<TPacket, FBBBMonsterStateAuthorityFactPacket>);
        return State;
    }
};
