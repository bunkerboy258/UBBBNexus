#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFacts.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterMontagePlaybackEntry.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterMontageSlots.h"
#include "BBBCharacterAnimationState.generated.h"

class UAnimInstance;

USTRUCT(BlueprintType)
//角色运行时动画数据
struct FBBBCharacterAnimationState final
{
    GENERATED_BODY()

    /** 当前帧提交给动画实例的角色事实 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationFacts Facts;

    /** 当前已经链接到角色主动画蓝图的动画层类 */
    UPROPERTY(Transient)
    TSubclassOf<UAnimInstance> LinkedAnimationLayerClass;

    /** 五个实际动画槽位各自只保存一个期望蒙太奇 */
    UPROPERTY(Transient)
    FBBBCharacterMontageSlots Slots;

    /** 已提交给引擎的蒙太奇播放记录 */
    UPROPERTY(Transient)
    TArray<FBBBCharacterMontagePlaybackEntry> ActiveMontages;

    /** 下一个蒙太奇播放修订号 */
    uint64 NextRevision = 1;

    /** 瞄准目标平滑速度 */
    FVector AimTargetSmoothVelocity = FVector::ZeroVector;

    /** 平滑后的组件空间瞄准目标 */
    FVector SmoothedAimTargetComponentSpace = FVector::ZeroVector;

    /** 平滑后的瞄准意图权重 */
    float SmoothedAimIntentAlpha = 0.0f;

    /** 是否已经建立有效平滑目标 */
    bool bHasSmoothedAimTarget = false;
};
