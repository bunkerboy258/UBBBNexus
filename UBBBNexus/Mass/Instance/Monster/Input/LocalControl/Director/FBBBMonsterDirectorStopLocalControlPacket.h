#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 停止导演 清空队列与运行中的预制波次 已生成小怪不受影响 */
struct FBBBMonsterDirectorStopLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    bool IsValid() const
    {
        return true;
    }

    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment&) const
    {
        return State.InstanceId.IsValid() && State.bRunning;
    }

    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State.bRunning = false;
        State.ActiveEvents.Reset();
        State.Queue.Reset();
        State.Reason = TEXT("已停止");
        Spawn.Reserved = 0;
        Spawn.Accounted = 0;
        ++Spawn.RequestId;
    }
};
