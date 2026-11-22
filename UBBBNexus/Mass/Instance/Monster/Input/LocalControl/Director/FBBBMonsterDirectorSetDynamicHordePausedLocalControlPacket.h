#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 暂停或恢复周期性动态波次 不影响固有区与预制波次 */
struct FBBBMonsterDirectorSetDynamicHordePausedLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    bool bPaused = false;

    bool IsValid() const
    {
        return true;
    }

    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment&) const
    {
        return State.InstanceId.IsValid();
    }

    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment&) const
    {
        State.bDynamicHordePaused = bPaused;
        if (!bPaused && State.NextDynamicHordeTime <= 0.0)
        {
            State.NextDynamicHordeTime = 0.0;
        }
    }
};
