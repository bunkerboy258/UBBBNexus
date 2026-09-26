#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRifleAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRiflePresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Animation/BBBRifleAnimInstance.h"

void FBBBRifleAnimationProcessor::Stop(FBBBRifleUpdateContext &Context)
{
    FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage, true);
    FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage, true);
    if (UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance())
    {
        Animation->Montage_Stop(0.1f);
    }

    Context.RuntimeData.Animation.AnimationState.bIsReloading = false;
}

void FBBBRifleAnimationProcessor::Update(FBBBRifleUpdateContext &Context)
{
    const auto &Action = Context.RuntimeData.Action.ReadRifleActionState();
    auto &State = Context.RuntimeData.Animation.AnimationState;
    if (Action.bEquippedThisFrame)
    {
        FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage);
    }

    // 第一次镜像快照只建立当前状态 不补播进入视野以前的射击
    if (State.FireSequence != Action.FireSequence && (State.bInitialized || !Context.Equipment.IsMirror()))
    {
        FBBBRiflePresentationProcessor::PlayFire(Context);
    }

    if (Action.bIsReloading && (!State.bIsReloading || State.ReloadSequence != Action.ReloadSequence))
    {
        FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage);
        FBBBRiflePresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
    }

    if (!Action.bIsReloading && State.bIsReloading)
    {
        FBBBRiflePresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage, true);
        UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance();
        if (Animation && Context.Definition.EquipmentReloadMontage)
        {
            Animation->Montage_Stop(0.1f, Context.Definition.EquipmentReloadMontage);
        }
    }

    State.FireSequence = Action.FireSequence;
    State.ReloadSequence = Action.ReloadSequence;
    State.bIsReloading = Action.bIsReloading;
    State.bInitialized = true;

    UBBBRifleAnimInstance *Animation = Cast<UBBBRifleAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("步枪动画蓝图必须继承 UBBBRifleAnimInstance")))
    {
        return;
    }

    Animation->PublishAnimationFacts(State.Pose);
    Animation->PublishRifleSnapshot(
        Action.LoadedAmmo,
        Action.AmmoCapacity,
        Action.bIsReloading,
        Context.World.GetTimeSeconds() - Action.LastFireTimeSeconds);
}
