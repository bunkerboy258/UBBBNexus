#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterAnimationInput.generated.h"
struct FBBBCharacterRuntimeData;

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

    /** @return 通知是否对应当前换弹操作 */
    bool CanApply(const FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    void Apply(FBBBCharacterRuntimeData &Data) const;
};
