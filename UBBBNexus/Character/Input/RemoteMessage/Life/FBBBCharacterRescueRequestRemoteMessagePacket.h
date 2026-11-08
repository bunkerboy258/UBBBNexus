#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 救援离散输入 */
struct FBBBCharacterRescueRequestRemoteMessagePacket final
{
    /** 请求来源 */
    TArray<TWeakObjectPtr<APawn>> RequestSources;
    /** 请求操作标识 */
    TArray<uint64> RequestOperations;
    /** 请求对应倒地轮次 */
    TArray<uint64> RequestRounds;
    /** 消息接收者 */
    TArray<TWeakObjectPtr<APawn>> RequestTargets;

    /** @return 数据是否完整 */
    bool IsValid() const
    {
        if (!(!RequestSources.IsEmpty() && RequestOperations.Num() == RequestSources.Num() && RequestRounds.Num() == RequestSources.Num() && RequestTargets.Num() == RequestSources.Num()))
        {
            return false;
        }
        for (int32 Index = 0; Index < RequestSources.Num(); ++Index)
        {
            if (RequestOperations[Index] == 0 || RequestRounds[Index] == 0)
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.RescueInbox.RequestSources.Append(RequestSources);
        Context.RescueInbox.RequestOperations.Append(RequestOperations);
        Context.RescueInbox.RequestRounds.Append(RequestRounds);
        Context.RescueInbox.RequestTargets.Append(RequestTargets);
    }
};
