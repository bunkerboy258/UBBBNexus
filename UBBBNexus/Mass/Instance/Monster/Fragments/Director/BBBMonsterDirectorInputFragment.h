#pragma once
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorInitializeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorStartLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorStopLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorSetDynamicHordePausedLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorTriggerEventWaveLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Director/FBBBMonsterDirectorStopEventWaveLocalControlPacket.h"
#include "BBBMonsterDirectorInputFragment.generated.h"

/** 每类导演输入独立覆盖槽 */
USTRUCT()
struct FBBBMonsterDirectorInputFragment final : public FMassFragment
{
    GENERATED_BODY()

    TBBBMassInputSlot<FBBBMonsterDirectorInitializeLocalControlPacket> Initialize;
    TBBBMassInputSlot<FBBBMonsterDirectorStartLocalControlPacket> Start;
    TBBBMassInputSlot<FBBBMonsterDirectorStopLocalControlPacket> Stop;
    TBBBMassInputSlot<FBBBMonsterDirectorSetDynamicHordePausedLocalControlPacket> SetDynamicHordePaused;
    TBBBMassInputSlot<FBBBMonsterDirectorTriggerEventWaveLocalControlPacket> TriggerEventWave;
    TBBBMassInputSlot<FBBBMonsterDirectorStopEventWaveLocalControlPacket> StopEventWave;

    /** @return 对应类型的唯一输入槽 */
    template<typename TPacket>
    auto& GetSlot()
    {
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorInitializeLocalControlPacket>)
        {
            return Initialize;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorStartLocalControlPacket>)
        {
            return Start;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorStopLocalControlPacket>)
        {
            return Stop;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorSetDynamicHordePausedLocalControlPacket>)
        {
            return SetDynamicHordePaused;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorTriggerEventWaveLocalControlPacket>)
        {
            return TriggerEventWave;
        }
        if constexpr (std::is_same_v<TPacket, FBBBMonsterDirectorStopEventWaveLocalControlPacket>)
        {
            return StopEventWave;
        }
    }
};

template<>
struct TMassFragmentTraits<FBBBMonsterDirectorInputFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
