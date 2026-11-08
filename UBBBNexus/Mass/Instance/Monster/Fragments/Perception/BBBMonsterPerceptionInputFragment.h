#pragma once

#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassInputSlot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Perception/FBBBMonsterSoundLocalControlPacket.h"
#include "BBBMonsterPerceptionInputFragment.generated.h"

/** 声音预留覆盖槽 本轮不接入玩家声音产生逻辑 */
USTRUCT()
struct FBBBMonsterPerceptionInputFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 仅保存最近一份待解析输入 */
    TBBBMassInputSlot<FBBBMonsterSoundLocalControlPacket> Sound;

    /** @return 对应声音输入槽 */
    template<typename TPacket>
    TBBBMassInputSlot<TPacket>& GetSlot()
    {
        static_assert(std::is_same_v<TPacket, FBBBMonsterSoundLocalControlPacket>);
        return Sound;
    }
};
