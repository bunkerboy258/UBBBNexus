#pragma once
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Spawn/FBBBMonsterSpawnRequestLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Spawn/FBBBMonsterSpawnPauseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Spawn/FBBBMonsterSpawnResumeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Spawn/FBBBMonsterSpawnStopLocalControlPacket.h"
#include "BBBMonsterSpawnInputFragment.generated.h"

/** 每个入口每种输入一个覆盖槽 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterSpawnInputFragment final : public FMassFragment
{
    GENERATED_BODY()
    TBBBMassInputSlot<FBBBMonsterSpawnRequestLocalControlPacket> Request;
    TBBBMassInputSlot<FBBBMonsterSpawnPauseLocalControlPacket> Pause;
    TBBBMassInputSlot<FBBBMonsterSpawnResumeLocalControlPacket> Resume;
    TBBBMassInputSlot<FBBBMonsterSpawnStopLocalControlPacket> Stop;

    /** @return 对应输入包的唯一覆盖槽 */
    template<typename TPacket>
    auto& GetSlot()
    {
        if constexpr (std::is_same_v<TPacket, FBBBMonsterSpawnRequestLocalControlPacket>)
        {
            return Request;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterSpawnPauseLocalControlPacket>)
        {
            return Pause;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterSpawnResumeLocalControlPacket>)
        {
            return Resume;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterSpawnStopLocalControlPacket>)
        {
            return Stop;
        }
    }
};

template<>
struct TMassFragmentTraits<FBBBMonsterSpawnInputFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
