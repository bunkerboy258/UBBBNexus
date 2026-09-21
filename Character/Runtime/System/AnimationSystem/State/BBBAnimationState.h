#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBCharacterMontagePlaybackState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBCharacterMontageSlotState.h"
#include "BBBAnimationState.generated.h"

class UAnimInstance;

USTRUCT(BlueprintType)
//角色运行时动画数据
struct FBBBAnimationState
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
    TArray<FBBBCharacterMontagePlaybackState> ActiveMontages;

    uint64 NextRevision = 1;
};
