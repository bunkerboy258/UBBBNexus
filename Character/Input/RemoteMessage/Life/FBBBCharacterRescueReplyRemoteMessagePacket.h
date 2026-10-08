#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 救援离散输入 */
struct FBBBCharacterRescueReplyRemoteMessagePacket final
{
    /** 回复来源 */
    TArray<TWeakObjectPtr<APawn>> ReplySources;
    /** 回复操作标识 */
    TArray<uint64> ReplyOperations;
    /** 回复倒地轮次 */
    TArray<uint64> ReplyRounds;
    /** 回复版本 */
    TArray<uint64> ReplyRevisions;
    /** 回复是否接受并继续 */
    TArray<bool> ReplyActive;
    /** 被救者确定的时长 */
    TArray<float> ReplyDurations;
    /** 回复结束原因 */
    TArray<FName> ReplyReasons;
    /** 消息接收者 */
    TArray<TWeakObjectPtr<APawn>> ReplyTargets;

    /** @return 数据是否完整 */
    bool IsValid() const
    {
        if (!(!ReplySources.IsEmpty() && ReplyOperations.Num() == ReplySources.Num() && ReplyRounds.Num() == ReplySources.Num() && ReplyRevisions.Num() == ReplySources.Num() && ReplyActive.Num() == ReplySources.Num() && ReplyDurations.Num() == ReplySources.Num() && ReplyReasons.Num() == ReplySources.Num() && ReplyTargets.Num() == ReplySources.Num()))
        {
            return false;
        }
        for (int32 Index = 0; Index < ReplySources.Num(); ++Index)
        {
            if (ReplyOperations[Index] == 0 || ReplyRounds[Index] == 0 || ReplyRevisions[Index] == 0 || !FMath::IsFinite(ReplyDurations[Index]) || ReplyDurations[Index] <= 0.0f)
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
        Context.RescueInbox.ReplySources.Append(ReplySources);
        Context.RescueInbox.ReplyOperations.Append(ReplyOperations);
        Context.RescueInbox.ReplyRounds.Append(ReplyRounds);
        Context.RescueInbox.ReplyRevisions.Append(ReplyRevisions);
        Context.RescueInbox.ReplyActive.Append(ReplyActive);
        Context.RescueInbox.ReplyDurations.Append(ReplyDurations);
        Context.RescueInbox.ReplyReasons.Append(ReplyReasons);
        Context.RescueInbox.ReplyTargets.Append(ReplyTargets);
    }
};
