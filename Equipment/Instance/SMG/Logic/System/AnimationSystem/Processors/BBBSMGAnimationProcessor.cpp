#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/Context/BBBSMGUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/RuntimeData/BBBSMGRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Animation/BBBSMGAnimInstance.h"

void FBBBSMGAnimationProcessor::Stop(FBBBSMGUpdateContext &Context)
{
    if (UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance())
    {
        Animation->Montage_Stop(0.1f);
    }

    Context.RuntimeData.Animation.AnimationState.bIsReloading = false;
}

void FBBBSMGAnimationProcessor::Update(FBBBSMGUpdateContext &Context)
{
    const auto &Action = Context.RuntimeData.Action.ReadSMGActionState();
    auto &State = Context.RuntimeData.Animation.AnimationState;
    if (Action.bEquippedThisFrame)
    {
        FBBBSMGPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage);
    }

    // 第一次镜像快照只建立当前状态 不补播进入视野以前的射击
    if (State.FireSequence != Action.FireSequence && (State.bInitialized || Context.bCausal))
    {
        FBBBSMGPresentationProcessor::PlayFire(Context);
    }

    if (Action.bIsReloading && (!State.bIsReloading || State.ReloadSequence != Action.ReloadSequence))
    {
        FBBBSMGPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage);
        FBBBSMGPresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
    }

    if (!Action.bIsReloading && State.bIsReloading)
    {
        // 装匣成功后让人物收手和武器动作自然结束 中断才立刻停止
        if (!Action.bReloadCompletedThisFrame)
        {
            FBBBSMGPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage, true);
            UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance();
            if (Animation && Context.Definition.EquipmentReloadMontage)
            {
                Animation->Montage_Stop(0.1f, Context.Definition.EquipmentReloadMontage);
            }
        }
    }

    State.FireSequence = Action.FireSequence;
    State.ReloadSequence = Action.ReloadSequence;
    State.bIsReloading = Action.bIsReloading;
    State.bInitialized = true;

    UBBBSMGAnimInstance *Animation = Cast<UBBBSMGAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("冲锋枪动画蓝图必须继承 UBBBSMGAnimInstance")))
    {
        return;
    }

    Animation->PublishAnimationFacts(State.Pose);
    Animation->PublishSMGSnapshot(
        Action.LoadedAmmo,
        Action.AmmoCapacity,
        Action.bIsReloading,
        FMath::Max(0.0, Context.World.GetTimeSeconds() - Action.LastFireTimeSeconds),
        Action.FireSequence,
        Context.World.GetTimeSeconds(),
        Context.Definition);
}
