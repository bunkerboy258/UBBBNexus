#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/Packets/BBBCharacterMontagePacket.h"
#include "BBBAnimationRuntimeData.generated.h"

class FBBBCharacterAnimationFactProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBCharacterAnimationSystem;
class FBBBCharacterInitializer;
class FBBBCharacterExternalAPI;
class FBBBCharacterAnimationActionProcessor;
class UAnimInstance;
class UBBBReloadMontagePlayback;

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
    friend class FBBBCharacterExternalAPI;
    friend class FBBBCharacterAnimationActionProcessor;

    /** 当前帧提交给动画实例的角色事实 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationFacts Facts;

    /** 当前已经链接到角色主动画蓝图的动画层类 */
    UPROPERTY(Transient)
    TSubclassOf<UAnimInstance> LinkedAnimationLayerClass;

    /** 当前帧尚未应用的人物蒙太奇贡献 */
    UPROPERTY(Transient)
    TArray<FBBBCharacterMontagePacket> MontageQueue;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UBBBReloadMontagePlayback>> ReloadPlaybacks;

    TArray<int32> CancelledReloadSequences;

};
