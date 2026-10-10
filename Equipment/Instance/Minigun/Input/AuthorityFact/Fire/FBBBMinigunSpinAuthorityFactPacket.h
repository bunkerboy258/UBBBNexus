#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionState.h"

/** 已成立的电机驱动状态 不同步逐帧枪管角度 */
struct FBBBMinigunSpinAuthorityFactPacket final
{
    /** 本帧收到的电机驱动结果 零停止 一驱动 */
    TArray<uint8> Spinning;

    /** 同一次持有期间的结果顺序 */
    TArray<uint64> Revisions;

    /** @return 结果数组具有合法值与对应顺序 */
    bool IsValid() const
    {
        return !Spinning.IsEmpty() && Spinning.Num() == Revisions.Num()
            && !Spinning.ContainsByPredicate([](uint8 Value)
            {
                return Value > 1;
            })
            && !Revisions.Contains(uint64(0));
    }

    /** @param State 当前动作事实 @return 是否存在更新的电机结果 */
    bool CanApply(const FBBBMinigunActionState &State) const
    {
        return Revisions.ContainsByPredicate([&State](uint64 Value)
        {
            return Value > State.SpinRevision;
        });
    }

    /** @param State 当前动作事实 @return 无 */
    void Apply(FBBBMinigunActionState &State) const
    {
        for (int32 Index = 0; Index < Spinning.Num(); ++Index)
        {
            if (Revisions[Index] <= State.SpinRevision)
            {
                continue;
            }
            State.SpinRevision = Revisions[Index];
            State.bIsSpinning = Spinning[Index] != 0;
        }
    }
};
