#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BBBCharacterRescueInboxState.generated.h"

/** 角色救援领域的独立状态 */
USTRUCT()
struct FBBBCharacterRescueInboxState final
{
    GENERATED_BODY()

    /** 请求来源 */
    TArray<TWeakObjectPtr<APawn>> RequestSources;

    /** 请求操作标识 */
    TArray<uint64> RequestOperations;

    /** 请求对应倒地轮次 */
    TArray<uint64> RequestRounds;

    /** 消息接收者 */
    TArray<TWeakObjectPtr<APawn>> RequestTargets;

    /** 取消来源 */
    TArray<TWeakObjectPtr<APawn>> CancelSources;

    /** 取消操作标识 */
    TArray<uint64> CancelOperations;

    /** 取消对应倒地轮次 */
    TArray<uint64> CancelRounds;

    /** 消息接收者 */
    TArray<TWeakObjectPtr<APawn>> CancelTargets;

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

};
