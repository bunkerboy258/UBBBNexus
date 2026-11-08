#pragma once

#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Input/LocalControl/Spawn/FBBBSlimeSpawnLocalControlPacket.h"
#include "BBBSlimeFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Input/AuthorityFact/Network/FBBBSlimeStateAuthorityFactPacket.h"
#include "BBBSlimeInputFragment.generated.h"

/** 粘液初始结果覆盖槽 */
USTRUCT()
struct FBBBSlimeInputFragment final : public FMassFragment
{
    GENERATED_BODY()
    TBBBMassInputSlot<FBBBSlimeSpawnLocalControlPacket> Spawn;
    TBBBMassInputSlot<FBBBSlimeStateAuthorityFactPacket> State;
    /** @return 粘液出生输入槽 */
    template<typename TPacket>
    TBBBMassInputSlot<TPacket>& GetSlot()
    {
        if constexpr (std::is_same_v<TPacket, FBBBSlimeSpawnLocalControlPacket>)
        {
            return Spawn;
        }
        else
        {
            static_assert(std::is_same_v<TPacket, FBBBSlimeStateAuthorityFactPacket>);
            return State;
        }
    }
};

/** 输入槽只保存当前纯值结果及对象引用 */
template<>
struct TMassFragmentTraits<FBBBSlimeInputFragment>
{
    enum { AuthorAcceptsItsNotTriviallyCopyable = true };
};
