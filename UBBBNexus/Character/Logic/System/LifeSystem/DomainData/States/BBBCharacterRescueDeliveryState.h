#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BBBCharacterRescueDeliveryState.generated.h"

/** 角色救援领域的独立状态 */
USTRUCT()
struct FBBBCharacterRescueDeliveryState final
{
    GENERATED_BODY()

    /** 本帧投送事实的递增标识 */
    uint64 Serial = 0;

    /** 请求接收者 */
    TArray<TWeakObjectPtr<APawn>> RequestTargets;

    /** 请求操作标识 */
    TArray<uint64> RequestOperations;

    /** 请求对应倒地轮次 */
    TArray<uint64> RequestRounds;

    /** 取消接收者 */
    TArray<TWeakObjectPtr<APawn>> CancelTargets;

    /** 取消操作标识 */
    TArray<uint64> CancelOperations;

    /** 取消对应倒地轮次 */
    TArray<uint64> CancelRounds;

    /** 回复接收者 */
    TArray<TWeakObjectPtr<APawn>> ReplyTargets;

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

};
