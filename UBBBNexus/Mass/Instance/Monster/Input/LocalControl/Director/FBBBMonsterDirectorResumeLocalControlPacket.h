#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 继续被暂停的副本阶段 */
struct FBBBMonsterDirectorResumeLocalControlPacket final
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
        return State.Phase == EBBBMonsterDirectorPhase::Paused;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State.Phase = State.ResumePhase;
        State.NextUpdateTime = 0.0;
        State.Reason = TEXT("继续当前阶段");
        ++State.Revision;
    }
};
