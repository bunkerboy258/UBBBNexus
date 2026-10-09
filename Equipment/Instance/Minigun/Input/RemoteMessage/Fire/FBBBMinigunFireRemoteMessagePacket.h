#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionState.h"

/** 已成立的转管机枪Fire结果 */
struct FBBBMinigunFireRemoteMessagePacket final
{
    /** 本帧收到的动作序号 */
    TArray<int32> Sequences;

    /** 同一次持有期间的事实顺序 */
    TArray<uint64> Revisions;

    /** @return 字段数量和动作序号有效 */
    bool IsValid() const
    {
        return !Sequences.IsEmpty() && Sequences.Num() == Revisions.Num()
            && !Sequences.ContainsByPredicate([](int32 Value)
            {
                return Value < 0;
            })
            && !Revisions.Contains(uint64(0));
    }

    /** @param State	本类装备当前动作事实 @return 是否有更新的结果 */
    bool CanApply(const FBBBMinigunActionState &State) const
    {
        return Revisions.ContainsByPredicate([&State](uint64 Value)
        {
            return Value > State.ActionRevision;
        });
    }

    /** @param State	本类装备的动作事实 @return 无 */
    void Apply(FBBBMinigunActionState &State) const
    {
        for (int32 Index = 0; Index < Sequences.Num(); ++Index)
        {
            if (Revisions[Index] <= State.ActionRevision)
            {
                continue;
            }

            State.ActionRevision = Revisions[Index];
            State.FireSequence = Sequences[Index];
            State.bIsReloading = false;
            State.bReloadCompletedThisFrame = false;
        }
    }
};
