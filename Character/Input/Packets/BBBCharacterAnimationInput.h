#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterAnimationInput.generated.h"

/** 动画报告的换弹阶段 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadAnimationPhase : uint8
{
    /** 卸下关键帧 */
    Start,
    /** 装填关键帧 */
    End,
    /** 播放中断或失败 */
    Interrupted
};

/** 结束入口的处理原因 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadEndReason : uint8
{
    /** 装填关键帧到达 */
    Loaded,
    /** 播放中断 */
    Interrupted,
    /** 播放失败或缺少结束通知 */
    PlaybackFailed
};

/** 角色动画输入，必须携带创建播放时的操作序号 */
USTRUCT(BlueprintType)
struct FBBBCharacterReloadAnimationInput
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 Sequence = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    EBBBCharacterReloadAnimationPhase Phase = EBBBCharacterReloadAnimationPhase::Start;

};
