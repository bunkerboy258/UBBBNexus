#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentAnimationStates.generated.h"

/** 装备表现动画的线程安全状态快照 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentAnimationState
{
    GENERATED_BODY()

    /** 最近一次开火序号 */
    UPROPERTY(BlueprintReadOnly)
    int32 FireSequence = 0;

    /** 最近一次开火世界时间 */
    UPROPERTY(BlueprintReadOnly)
    float LastFireTimeSeconds = -1000.0f;

    /** 最近一次换弹序号 */
    UPROPERTY(BlueprintReadOnly)
    int32 ReloadSequence = 0;

    /** 当前是否处于换弹动作 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsReloading = false;

    /** 换弹开始世界时间 */
    UPROPERTY(BlueprintReadOnly)
    float ReloadStartTimeSeconds = 0.0f;

    /** 当前换弹动作持续时间 */
    UPROPERTY(BlueprintReadOnly)
    float ReloadDurationSeconds = 0.0f;

    /** 当前动画实例采样到的世界时间 */
    UPROPERTY(BlueprintReadOnly)
    float CurrentWorldTimeSeconds = 0.0f;

    /** 当前开火动作是否仍在激活窗口内 */
    UPROPERTY(BlueprintReadOnly)
    bool bFireActive = false;

    /** 最近一次开火经过的时间 */
    UPROPERTY(BlueprintReadOnly)
    float FireElapsedSeconds = 1000.0f;

    /** 当前换弹动作经过的时间 */
    UPROPERTY(BlueprintReadOnly)
    float ReloadElapsedSeconds = 0.0f;
};
