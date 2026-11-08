#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BBBCharacterRescueState.generated.h"

/** 角色救援领域的独立状态 */
USTRUCT()
struct FBBBCharacterRescueState final
{
    GENERATED_BODY()

    /** 当前救援关系中的另一角色 */
    TWeakObjectPtr<APawn> Partner;

    /** 救援者生成的操作标识 结束后保留 */
    uint64 OperationId = 0;

    /** 仅由本角色生成的操作计数 不被接受他人请求覆盖 */
    uint64 LocalOperationCounter = 0;

    /** 目标本轮倒地标识 */
    uint64 DownedRevision = 0;

    /** 本角色救援结果版本 */
    uint64 Revision = 0;

    /** 当前操作已消费的回复版本 */
    uint64 ReplyRevision = 0;

    /** 本角色世界中的进度时间基准 */
    double StartTime = 0.0;

    /** 被救者确定的救援时长 */
    float Duration = 3.0f;

    /** 只读表现进度 */
    float Progress = 0.0f;

    /** 正在发起或执行帮扶 */
    bool bHelping = false;

    /** 正在接受帮扶 */
    bool bReceiving = false;

    /** 被救者已经接受当前操作 */
    bool bAccepted = false;

    /** 等待生命处理器提交恢复结果 */
    bool bCompletionPending = false;

    /** 最后一次结束原因 */
    FName EndReason = NAME_None;

    /** 各救援者当前已消费请求标识 防止旧请求重新占用 */
    TMap<TWeakObjectPtr<APawn>, uint64> RequestOperations;

};
