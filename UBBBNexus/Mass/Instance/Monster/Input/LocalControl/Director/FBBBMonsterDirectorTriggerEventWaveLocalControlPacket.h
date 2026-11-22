#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 触发配置中的具名预制事件波次 */
struct FBBBMonsterDirectorTriggerEventWaveLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    /** 要触发的事件标识 */
    FName EventId;
    /** 可选的指定初始目标玩家编号 为 INDEX_NONE 时自动锁定最近玩家 */
    int32 InitialTargetPlayerId = INDEX_NONE;

    bool IsValid() const
    {
        return !EventId.IsNone() && InitialTargetPlayerId >= INDEX_NONE;
    }

    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment&) const
    {
        if (!State.InstanceId.IsValid() || !State.bRunning || !State.Definition.IsValid())
        {
            return false;
        }
        return State.Definition->FindEventWave(EventId) != nullptr;
    }

    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment&) const
    {
        for (auto& Existing : State.ActiveEvents)
        {
            if (Existing.EventId == EventId)
            {
                Existing.Spawned = 0;
                Existing.NextSpawnTime = 0.0;
                Existing.InitialTargetPlayerId = InitialTargetPlayerId;
                State.Reason = FString::Printf(TEXT("重新触发预制波次 %s"), *EventId.ToString());
                return;
            }
        }
        FBBBMonsterDirectorActiveEvent Event;
        Event.EventId = EventId;
        Event.Spawned = 0;
        Event.NextSpawnTime = 0.0;
        Event.InitialTargetPlayerId = InitialTargetPlayerId;
        State.ActiveEvents.Add(Event);
        State.Reason = FString::Printf(TEXT("已触发预制波次 %s"), *EventId.ToString());
    }
};
