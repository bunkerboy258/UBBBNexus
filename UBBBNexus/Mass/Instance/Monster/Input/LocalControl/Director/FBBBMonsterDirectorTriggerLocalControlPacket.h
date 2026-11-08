#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 触发当前等待中的副本阶段 */
struct FBBBMonsterDirectorTriggerLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    /** 当前副本运行代号 */
    uint32 Generation = 0;
    /** 明确触发的阶段 */
    int32 StageIndex = INDEX_NONE;

    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return Generation > 0 && StageIndex >= 0;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 当前输入能否应用
     */
    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        return State.Generation == Generation && State.StageIndex == StageIndex && State.Phase == EBBBMonsterDirectorPhase::Waiting;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State.Phase = EBBBMonsterDirectorPhase::Spawning;
        State.NextUpdateTime = 0.0;
        State.Reason = TEXT("阶段已触发");
        ++State.Revision;
    }
};
