#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 停止指定预制事件波次 EventId 为 None 时停止全部运行中的预制波次 */
struct FBBBMonsterDirectorStopEventWaveLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    /** 要停止的事件标识 None 表示全部停止 */
    FName EventId;

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
        if (EventId.IsNone())
        {
            State.ActiveEvents.Reset();
            State.Queue.RemoveAll([](const FBBBMonsterDirectorQueuedRequest& Item)
            {
                return Item.Source == EBBBMonsterSpawnSource::EventWave;
            });
            State.Reason = TEXT("已停止全部预制波次");
            return;
        }
        State.ActiveEvents.RemoveAll([Id = EventId](const FBBBMonsterDirectorActiveEvent& Event)
        {
            return Event.EventId == Id;
        });
        State.Queue.RemoveAll([Id = EventId](const FBBBMonsterDirectorQueuedRequest& Item)
        {
            return Item.Source == EBBBMonsterSpawnSource::EventWave && Item.EventId == Id;
        });
        State.Reason = FString::Printf(TEXT("已停止预制波次 %s"), *EventId.ToString());
    }
};
