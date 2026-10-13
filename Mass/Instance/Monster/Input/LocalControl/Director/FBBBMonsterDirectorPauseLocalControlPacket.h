#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 暂停导演并取消当前出生申请 */
struct FBBBMonsterDirectorPauseLocalControlPacket final
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
        return State.Phase == EBBBMonsterDirectorPhase::Waiting || State.Phase == EBBBMonsterDirectorPhase::Spawning
            || State.Phase == EBBBMonsterDirectorPhase::Clearing;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State.ResumePhase = State.Phase;
        State.Phase = EBBBMonsterDirectorPhase::Paused;
        State.Reason = TEXT("已暂停 新增申请已取消");
        ++State.Revision;
        ++Spawn.RequestId;
        Spawn.Reserved = 0;
        Spawn.bNeedsQuery = false;
        Spawn.bResultReady = false;
        Spawn.bCreating = false;
        Spawn.Locations.Reset();
    }
};
