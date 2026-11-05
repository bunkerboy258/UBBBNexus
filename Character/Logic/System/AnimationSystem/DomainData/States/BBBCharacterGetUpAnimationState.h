#pragma once
#include "CoreMinimal.h"
#include "Animation/PoseSnapshot.h"
#include "BBBCharacterGetUpAnimationState.generated.h"

/** 布娃娃起身的选片与姿态交接结果 不维护玩法阶段 */
USTRUCT()
struct FBBBCharacterGetUpAnimationState final
{
    GENERATED_BODY()
    /** 选片所属物理轮次 */
    uint64 SelectedEpisode = 0;
    /** 已提交动作的阶段版本 */
    uint64 AppliedTransition = 0;
    /** 已选择的起身蒙太奇 */
    UPROPERTY(Transient)
    TObjectPtr<class UAnimMontage> SelectedMontage = nullptr;
    /** 姿态匹配返回的动作起点 */
    float SelectedTime = 0.0f;
    /** 原始物理姿态 只用于退出物理的混合 */
    UPROPERTY(Transient)
    FPoseSnapshot RecoveryPose;
    /** 退出物理姿态是否仍参与起身淡入 */
    bool bUseRecoveryPose = false;
    /** 捕获姿态时的网格世界变换 */
    FTransform CaptureMeshTransform;
    /** 进入恢复前的根运动模式 用于动作结束后还原 */
    uint8 PreviousRootMotionMode = 0;
    /** 是否已接管起身根运动设置 */
    bool bRootMotionOverride = false;
};
