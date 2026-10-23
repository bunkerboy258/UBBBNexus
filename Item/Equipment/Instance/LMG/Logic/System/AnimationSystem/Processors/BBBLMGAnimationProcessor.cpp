#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/Processors/BBBLMGAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/DomainData/Context/BBBLMGAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/RuntimeData/BBBLMGRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/Processors/BBBLMGPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Animation/BBBLMGAnimInstance.h"

void FBBBLMGAnimationProcessor::Stop(FBBBLMGAnimationUpdateContext &Context)
{
    if (UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance())
    {
        Animation->Montage_Stop(0.1f);
    }

    Context.RuntimeData.Animation.AnimationState.bIsReloading = false;
}

void FBBBLMGAnimationProcessor::Update(FBBBLMGAnimationUpdateContext &Context)
{
    Context.Equipment.GetEquipmentSkeletalMesh()->SetHiddenInGame(Context.RuntimeData.Action.ReadLMGActionState().bHide);

    const auto &Action = Context.RuntimeData.Action.ReadLMGActionState();
    auto &State = Context.RuntimeData.Animation.AnimationState;
    if (Action.bEquippedThisFrame)
    {
        FBBBLMGPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterEquipMontage);
    }

    // 第一次镜像快照只建立当前状态 不补播进入视野以前的射击
    if (State.FireSequence != Action.FireSequence && (State.bInitialized || Context.bCausal))
    {
        FBBBLMGPresentationProcessor::PlayFire(Context);
    }

    if (Action.bIsReloading && (!State.bIsReloading || State.ReloadSequence != Action.ReloadSequence))
    {
        FBBBLMGPresentationProcessor::SubmitCharacterMontage(Context, Context.Definition.CharacterReloadMontage);
        FBBBLMGPresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);
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

    UBBBLMGAnimInstance *Animation = Cast<UBBBLMGAnimInstance>(Context.Equipment.GetEquipmentAnimationInstance());
    if (!ensureMsgf(Animation, TEXT("轻机枪动画蓝图必须继承 UBBBLMGAnimInstance")))
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
    Animation->PublishLMGSnapshot(
        Action.LoadedAmmo,
        Action.AmmoCapacity,
        Action.bIsReloading,
        FMath::Max(0.0, Context.World.GetTimeSeconds() - Action.LastFireTimeSeconds),
        Action.FireSequence,
        Context.World.GetTimeSeconds(),
        Context.Definition);
}
