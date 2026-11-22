#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 启动导演 启用固有区激活 动态波次与预制波次 */
struct FBBBMonsterDirectorStartLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    bool IsValid() const
    {
        return true;
    }

    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment&) const
    {
        return State.InstanceId.IsValid() && State.Definition.IsValid() && !State.bRunning;
    }

    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        ++State.Generation;
        State.bRunning = true;
        State.TotalSpawned = 0;
        State.ActivatedPopulationRegionIds.Reset();
        State.ActiveEvents.Reset();
        State.Queue.Reset();
        State.NextDynamicHordeTime = 0.0;
        State.NextUpdateTime = 0.0;
        State.Reason = TEXT("导演运行中");
        Spawn.Reserved = 0;
        Spawn.Accounted = 0;
        ++Spawn.RequestId;
    }
};
