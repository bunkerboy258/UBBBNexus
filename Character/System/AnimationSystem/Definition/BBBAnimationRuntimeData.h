#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBAnimationRuntimeData.generated.h"

class FBBBCharacterAnimationFactProcessor;
class FBBBCharacterAnimationSystem;
class FBBBCharacterInitializer;
class UAnimInstance;

USTRUCT(BlueprintType)
//角色运行时动画数据
struct FBBBAnimationRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterAnimationSystem;
    friend class FBBBCharacterAnimationFactProcessor;
    friend class FBBBCharacterInitializer;

    /** 当前帧提交给动画实例的角色事实 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationFacts Facts;

    /** 当前已经链接到角色主动画蓝图的动画层类 */
    UPROPERTY(Transient)
    TSubclassOf<UAnimInstance> LinkedAnimationLayerClass;

    /** 装备通过角色接口请求链接的动画层类 */
    UPROPERTY(Transient)
    TSubclassOf<UAnimInstance> RequestedAnimationLayerClass;
};
