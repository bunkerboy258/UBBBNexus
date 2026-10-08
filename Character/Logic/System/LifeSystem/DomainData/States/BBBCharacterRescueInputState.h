#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BBBCharacterRescueInputState.generated.h"

/** 角色救援领域的独立状态 */
USTRUCT()
struct FBBBCharacterRescueInputState final
{
    GENERATED_BODY()

    /** 本帧按下救援键 */
    bool bBegin = false;

    /** 本帧释放救援键 */
    bool bCancel = false;

    /** 请求来源 */
    TArray<TWeakObjectPtr<APawn>> RequestSources;

    /** 请求操作标识 */
    TArray<uint64> RequestOperations;

    /** 请求对应倒地轮次 */
    TArray<uint64> RequestRounds;

    /** 取消来源 */
    TArray<TWeakObjectPtr<APawn>> CancelSources;

    /** 取消操作标识 */
    TArray<uint64> CancelOperations;

    /** 取消对应倒地轮次 */
    TArray<uint64> CancelRounds;

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

    /** 镜像结果伙伴 */
    TArray<TWeakObjectPtr<APawn>> SnapshotPartners;

    /** 镜像结果操作 */
    TArray<uint64> SnapshotOperations;

    /** 镜像结果倒地轮次 */
    TArray<uint64> SnapshotRounds;

    /** 镜像结果版本 */
    TArray<uint64> SnapshotRevisions;

    /** 镜像帮扶结果 */
    TArray<bool> SnapshotHelping;

    /** 镜像被救结果 */
    TArray<bool> SnapshotReceiving;

    /** 镜像当前请求已被接受 */
    TArray<bool> SnapshotAccepted;

    /** 镜像当前已进行时间 */
    TArray<float> SnapshotElapsed;

    /** 镜像时长 */
    TArray<float> SnapshotDurations;

    /** 镜像结束原因 */
    TArray<FName> SnapshotReasons;

};
