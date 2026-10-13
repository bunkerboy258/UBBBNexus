#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 停止新增 保留已出生小怪的现有生命周期 */
struct FBBBMonsterDirectorStopLocalControlPacket final
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
        return State.InstanceId.IsValid() && State.Phase != EBBBMonsterDirectorPhase::Stopped;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State.Phase = EBBBMonsterDirectorPhase::Stopped;
        State.Reason = TEXT("已停止 已出生小怪继续当前生命周期");
        ++State.Revision;
        ++Spawn.RequestId;
        Spawn.Reserved = 0;
        Spawn.bNeedsQuery = false;
        Spawn.bResultReady = false;
        Spawn.bCreating = false;
        Spawn.Locations.Reset();
    }
};
