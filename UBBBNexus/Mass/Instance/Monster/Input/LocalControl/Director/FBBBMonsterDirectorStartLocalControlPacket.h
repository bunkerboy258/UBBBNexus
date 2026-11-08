#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 开始一次新的副本运行 */
struct FBBBMonsterDirectorStartLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;


    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 当前输入能否应用
     */
    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        const bool bTerminal = State.Phase == EBBBMonsterDirectorPhase::Inactive || State.Phase == EBBBMonsterDirectorPhase::Stopped
            || State.Phase == EBBBMonsterDirectorPhase::Completed;
        return State.InstanceId.IsValid() && State.Definition.IsValid() && bTerminal && State.EntityCount == 0 && Spawn.Reserved == 0;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        ++State.Generation;
        ++State.Revision;
        State.StageIndex = 0;
        State.StageSpawned = 0;
        State.TotalSpawned = 0;
        State.Phase = EBBBMonsterDirectorPhase::Waiting;
        State.NextSpawnTime = 0.0;
        State.NextUpdateTime = 0.0;
        State.Reason = TEXT("等待阶段条件");
    }
};
