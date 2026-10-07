#pragma once
#include "CoreMinimal.h"

#include "BBBCharacterTraversalAnimationState.generated.h"

class UAnimMontage;

/** 翻越动画请求的独立去重生命周期 */
USTRUCT()
struct FBBBCharacterTraversalAnimationState final
{
    GENERATED_BODY()
    /** 上次已交给蓝图选择动画的动作序号 */
    uint32 LastActionId = 0;

    /** 本动作实际使用的动画 用于定向中断而不误停接替动作 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** 实例序号用于区分同一动画的连续两次播放 */
    int32 MontageInstanceId = INDEX_NONE;

    /** 最后一个攀爬校正窗口的结束位置 */
    float LastWarpEndTime = 0.0f;

    /** 手部接触校正完成后才允许翻越在障碍背面转为自由下落 */
    float ContactWarpEndTime = 0.0f;

    /** 当前实际播放位置 */
    float Position = 0.0f;

    /** 本动作动画是否仍在运行 */
    bool bPlaying = false;
    /** 本动作仍对姿势有贡献 包括已停止的淡出尾段 */
    bool bPoseActive = false;

    /** 本动作已经不再贡献根运动 */
    bool bRootMotionReleased = false;

    /** 最后一个攀爬校正窗口已经完成 */
    bool bExitWindowReached = false;
};
