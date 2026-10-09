#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/Context/BBBSniperUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/RuntimeData/BBBSniperRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Animation/BBBSniperAnimInstance.h"

void FBBBSniperAnimationProcessor::Stop(FBBBSniperUpdateContext &Context)
{
    if (UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance())
    {
        Animation->Montage_Stop(0.1f);
    }

    Context.RuntimeData.Animation.AnimationState.bIsReloading = false;
}

void FBBBSniperAnimationProcessor::Update(FBBBSniperUpdateContext &Context)
{
    const auto &Action = Context.RuntimeData.Action.ReadSniperActionState();
    auto &State = Context.RuntimeData.Animation.AnimationState;
    if (Action.bEquippedThisFrame)
    {
        FBBBSniperPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage);
    }

    // 第一次镜像快照只建立当前状态 不补播进入视野以前的射击
    if (State.FireSequence != Action.FireSequence && (State.bInitialized || Context.bCausal))
    {
        FBBBSniperPresentationProcessor::PlayFire(Context);
    }

    if (Action.bIsReloading && (!State.bIsReloading || State.ReloadSequence != Action.ReloadSequence))
    {
        FBBBSniperPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage);
        FBBBSniperPresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
    }

    if (!Action.bIsReloading && State.bIsReloading)
    {
        // 装匣成功后让人物收手和武器动作自然结束 中断才立刻停止
        if (!Action.bReloadCompletedThisFrame)
        {
            FBBBSniperPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage, true);
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

    UBBBSniperAnimInstance *Animation = Cast<UBBBSniperAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("狙击枪动画蓝图必须继承 UBBBSniperAnimInstance")))
    {
        return;
    }

    Animation->PublishAnimationFacts(State.Pose);
    Animation->PublishSniperSnapshot(
        Action.LoadedAmmo,
        Action.AmmoCapacity,
        Action.bIsReloading,
        FMath::Max(0.0, Context.World.GetTimeSeconds() - Action.LastFireTimeSeconds),
        Action.FireSequence,
        Context.World.GetTimeSeconds(),
        Context.Definition);
}
