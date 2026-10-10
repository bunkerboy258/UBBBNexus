#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/Processors/BBBMinigunAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/Context/BBBMinigunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/RuntimeData/BBBMinigunRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/Processors/BBBMinigunPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Animation/BBBMinigunAnimInstance.h"

void FBBBMinigunAnimationProcessor::Stop(FBBBMinigunUpdateContext &Context)
{
    if (UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance())
    {
        Animation->Montage_Stop(0.1f);
    }

    Context.RuntimeData.Animation.AnimationState.bIsReloading = false;
    if (UBBBMinigunAnimInstance *Animation = Cast<UBBBMinigunAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance()))
    {
        Animation->bIsSpinning = false;
    }
}

void FBBBMinigunAnimationProcessor::Update(FBBBMinigunUpdateContext &Context)
{
    const auto &Action = Context.RuntimeData.Action.ReadMinigunActionState();
    auto &State = Context.RuntimeData.Animation.AnimationState;
    if (Action.bEquippedThisFrame)
    {
        FBBBMinigunPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage);
    }

    // 第一次镜像快照只建立当前状态 不补播进入视野以前的射击
    if (State.FireSequence != Action.FireSequence && (State.bInitialized || Context.bCausal))
    {
        FBBBMinigunPresentationProcessor::PlayFire(Context);
    }

    if (Action.bIsReloading && (!State.bIsReloading || State.ReloadSequence != Action.ReloadSequence))
    {
        FBBBMinigunPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage);
        FBBBMinigunPresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
    }

    if (!Action.bIsReloading && State.bIsReloading)
    {
        // 装匣成功后让人物收手和武器动作自然结束 中断才立刻停止
        if (!Action.bReloadCompletedThisFrame)
        {
            FBBBMinigunPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage, true);
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

    UBBBMinigunAnimInstance *Animation = Cast<UBBBMinigunAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("转管机枪动画蓝图必须继承 UBBBMinigunAnimInstance")))
    {
        return;
    }

    Animation->PublishAnimationFacts(State.Pose);
    Animation->PublishMinigunSnapshot(
        Action.LoadedAmmo,
        Action.AmmoCapacity,
        Action.bIsReloading,
        Action.bIsSpinning,
        FMath::Max(0.0, Context.World.GetTimeSeconds() - Action.LastFireTimeSeconds),
        Action.FireSequence,
        Context.World.GetTimeSeconds(),
        Context.Definition);
}
