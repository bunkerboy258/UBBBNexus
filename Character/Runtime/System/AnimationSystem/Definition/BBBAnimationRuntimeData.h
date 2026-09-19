#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageSlotState.h"
#include "BBBAnimationRuntimeData.generated.h"

class FBBBCharacterAnimationFactProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBCharacterAnimationSystem;
class FBBBCharacterInitializer;
class FBBBCharacterAnimationActionProcessor;
class UAnimInstance;
class UBBBMontagePlayback;

USTRUCT(BlueprintType)
//角色运行时动画数据
struct FBBBAnimationRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterAnimationSystem;
    friend class FBBBCharacterAnimationFactProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterAnimationActionProcessor;
    friend struct FBBBCharacterMontagePacket;

    /** 当前帧提交给动画实例的角色事实 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationFacts Facts;

    /** 当前已经链接到角色主动画蓝图的动画层类 */
    UPROPERTY(Transient)
    TSubclassOf<UAnimInstance> LinkedAnimationLayerClass;

    /** 每个实际动画槽位只保存一个期望蒙太奇 */
    UPROPERTY(Transient)
    TArray<FBBBCharacterMontageSlotState> Slots;

    /** 一个播放对象可以同时占用多个槽位 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UBBBMontagePlayback>> Playbacks;

    uint64 NextRevision = 1;
};
