#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionState.h"

/** 已成立的冲锋枪ReloadEnd结果 */
struct FBBBSMGReloadEndRemoteMessagePacket final
{
    /** 本帧收到的动作序号 */
    TArray<int32> Sequences;

    /** 同一次持有期间的事实顺序 */
    TArray<uint64> Revisions;

    /** 是否自然装填完成 */
    TArray<bool> Completed;

    /** @return 字段数量和动作序号有效 */
    bool IsValid() const
    {
        return !Sequences.IsEmpty() && Sequences.Num() == Revisions.Num() && Completed.Num() == Sequences.Num()
            && !Sequences.ContainsByPredicate([](int32 Value)
            {
                return Value < 0;
            })
            && !Revisions.Contains(uint64(0));
    }

    /** @param State	本类装备当前动作事实 @return 是否有更新的结果 */
    bool CanApply(const FBBBSMGActionState &State) const
    {
        return Revisions.ContainsByPredicate([&State](uint64 Value)
        {
            return Value > State.ActionRevision;
        });
    }

    /** @param State	本类装备的动作事实 @return 无 */
    void Apply(FBBBSMGActionState &State) const
    {
        for (int32 Index = 0; Index < Sequences.Num(); ++Index)
        {
            if (Revisions[Index] <= State.ActionRevision)
            {
                continue;
            }

            State.ActionRevision = Revisions[Index];
            State.ReloadSequence = Sequences[Index];
            State.bIsReloading = false;
            State.bReloadCompletedThisFrame = Completed[Index];
        }
    }
};
