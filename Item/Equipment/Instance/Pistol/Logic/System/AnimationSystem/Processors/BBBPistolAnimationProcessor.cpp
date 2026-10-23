#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/Processors/BBBPistolAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/DomainData/Context/BBBPistolAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/RuntimeData/BBBPistolRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/Processors/BBBPistolPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Animation/BBBPistolAnimInstance.h"

void FBBBPistolAnimationProcessor::Stop(FBBBPistolAnimationUpdateContext &Context)
{
    if (UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance())
    {
        Animation->Montage_Stop(0.1f);
    }

    Context.RuntimeData.Animation.AnimationState.bIsReloading = false;
}

void FBBBPistolAnimationProcessor::Update(FBBBPistolAnimationUpdateContext &Context)
{
    Context.Equipment.GetEquipmentSkeletalMesh()->SetHiddenInGame(Context.RuntimeData.Action.ReadPistolActionState().bHide);

    const auto &Action = Context.RuntimeData.Action.ReadPistolActionState();
    auto &State = Context.RuntimeData.Animation.AnimationState;
    if (Action.bEquippedThisFrame)
    {
        FBBBPistolPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage);
    }

    // 第一次镜像快照只建立当前状态 不补播进入视野以前的射击
    if (State.FireSequence != Action.FireSequence && (State.bInitialized || Context.bCausal))
    {
        FBBBPistolPresentationProcessor::PlayFire(Context);
    }

    if (Action.bIsReloading && (!State.bIsReloading || State.ReloadSequence != Action.ReloadSequence))
    {
        FBBBPistolPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage);
        FBBBPistolPresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
    }

    if (!Action.bIsReloading && State.bIsReloading)
    {
        // 装匣成功后让武器动作自然结束 中断只停止武器自身动画
        if (!Action.bReloadCompletedThisFrame)
        {
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

    UBBBPistolAnimInstance *Animation = Cast<UBBBPistolAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("手枪动画蓝图必须继承 UBBBPistolAnimInstance")))
    {
        return;
    }

    Animation->PublishRecoilFacts(
        Context.Definition.RecoilPattern, Context.Definition.RecoilReturnSpeed,
        Context.Definition.HipFireSettings.RecoilScale, Context.Definition.AimFireSettings.RecoilScale,
        Context.Definition.CrouchedModifiers.RecoilScale, Context.Definition.AirborneModifiers.RecoilScale,
        Context.Definition.bAutomaticFire, Action.LoadedAmmo > 0 && !Action.bFireBlocked && !Action.bIsReloading,
        Action.bEquippedThisFrame);
    Animation->PublishAnimationFacts(State.Pose);
    Animation->PublishPistolSnapshot(
        Action.LoadedAmmo,
        Action.AmmoCapacity,
        Action.bIsReloading,
        FMath::Max(0.0, Context.World.GetTimeSeconds() - Action.LastFireTimeSeconds),
        Action.FireSequence,
        Context.World.GetTimeSeconds(),
        Context.Definition);
}
