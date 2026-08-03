
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationProcessor.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationState.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

//播放排队蒙太奇并更新开火与转身表现
void FBBBCharacterAnimationProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    const FBBBCharacterAnimationCommands &AnimationCommands,
    FBBBCharacterAnimationState &AnimationState,
    const FBBBIntentRuntimeData &IntentData,
    const FBBBFacingRuntimeData &FacingData,
    const FBBBAimRuntimeData &AimData) const
{

    //读取动画实例与待播蒙太奇队列
    UAnimInstance *AnimInstance = CharacterMesh.GetAnimInstance();
    const TArray<FBBBCharacterAnimationRequest> &PendingMontages = AnimationCommands.GetPendingMontages();
    //有命令但无动画实例时报警丢弃
    if (!PendingMontages.IsEmpty() && !AnimInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Discarded animation commands without animation instance"));
    }
    //逐条消费蒙太奇请求
    if (AnimInstance)
    {
        for (const FBBBCharacterAnimationRequest &Request : PendingMontages)
        {
            if (!Request.Montage)
            { continue; }
            //允许打断时先停止同名蒙太奇
            if (Request.bRestartIfPlaying && AnimInstance->Montage_IsPlaying(Request.Montage))
            {
                AnimInstance->Montage_Stop(0.0f, Request.Montage);
            }

            AnimInstance->Montage_Play(Request.Montage);
        }
    }

    //读取瞄准状态
    const FBBBAimRuntimeState &AimState = AimData.GetState();

    //提交开火与原地转身表现
    AnimationState.bWantsFire = IntentData.WantsFire();
    AnimationState.bIsTurningInPlaceLeft = FacingData.IsBodyTurning()
        && AimState.AimYaw < 0.0f;
    AnimationState.bIsTurningInPlaceRight = FacingData.IsBodyTurning()
        && AimState.AimYaw > 0.0f;
}
