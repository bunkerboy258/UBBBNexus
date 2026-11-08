#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 救援离散输入 */
struct FBBBCharacterRescueEndLocalControlPacket final
{
    /** 取消来源 */
    TArray<TWeakObjectPtr<APawn>> CancelSources;
    /** 取消操作标识 */
    TArray<uint64> CancelOperations;
    /** 取消对应倒地轮次 */
    TArray<uint64> CancelRounds;

    /** @return 数据是否完整 */
    bool IsValid() const
    {
        if (!(!CancelSources.IsEmpty() && CancelOperations.Num() == CancelSources.Num() && CancelRounds.Num() == CancelSources.Num()))
        {
            return false;
        }
        for (int32 Index = 0; Index < CancelSources.Num(); ++Index)
        {
            if (CancelOperations[Index] == 0 || CancelRounds[Index] == 0)
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.RescueInputs.CancelSources.Append(CancelSources);
        Context.RescueInputs.CancelOperations.Append(CancelOperations);
        Context.RescueInputs.CancelRounds.Append(CancelRounds);
    }
};
