#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterLifeAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterRescueCompleteLogicAnimNotifyState.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLifeAnimationProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.LifeAnimationState;
    const auto &Life = Context.RuntimeData.Life.ReadLifeState();
    const auto &Rescue = Context.RuntimeData.Life.ReadRescueState();
    const auto &Config = Context.AnimationConfig;
    if (!Life.bInitialized)
    {
        return;
    }
    const EBBBCharacterLifePhase PreviousPhase = State.AppliedPhase;
    const bool bWasHelping = State.bAppliedRescueHelping;
    const bool bWasReceiving = State.bAppliedRescueReceiving;
    const uint64 Operation = Rescue.bHelping || Rescue.bReceiving ? Rescue.OperationId : 0;
    const bool bChanged = State.AppliedPhase != Life.Phase
        || State.bAppliedRescueHelping != Rescue.bHelping
        || State.bAppliedRescueReceiving != Rescue.bReceiving
        || State.AppliedRescueOperation != Operation;
    const auto *Movement = Context.Character.GetCharacterMovement();
    const bool bMoving = !Context.RuntimeData.Parse.ReadControlState().MoveWorld.IsNearlyZero()
        || (Movement && Movement->Velocity.SizeSquared2D() > 25.0f);
    UAnimMontage *Current = Context.AnimationInstance.FullBodyMontageContribution.Get();
    const bool bRecovering = Current && (Current == Config.RecoverStandMontage || Current == Config.RecoverCrouchMontage)
        && Context.AnimationInstance.Montage_IsActive(Current);
    if (!bChanged && !(Life.Phase == EBBBCharacterLifePhase::Alive && !Rescue.bHelping && bRecovering && bMoving))
    {
        return;
    }
    State.AppliedPhase = Life.Phase;
    State.bAppliedRescueHelping = Rescue.bHelping;
    State.bAppliedRescueReceiving = Rescue.bReceiving;
    State.AppliedRescueOperation = Operation;

    UAnimMontage *Desired = Config.LifeActionReturnMontage.Get();
    if (Life.Phase == EBBBCharacterLifePhase::Downed || Life.Phase == EBBBCharacterLifePhase::Dead)
    {
        Desired = Life.Phase == EBBBCharacterLifePhase::Downed && Rescue.bReceiving
            ? Config.ReceiveRescueMontage.Get() : Config.DownedHoldMontage.Get();
    }
    if (Life.Phase == EBBBCharacterLifePhase::Alive && Rescue.bHelping)
    {
        Desired = Config.HelpRescueMontage.Get();
    }
    if (Life.Phase == EBBBCharacterLifePhase::Alive && !Rescue.bHelping && PreviousPhase == EBBBCharacterLifePhase::Downed && !bMoving)
    {
        Desired = Life.bRecoveryCrouched ? Config.RecoverCrouchMontage.Get() : Config.RecoverStandMontage.Get();
    }
    if (Life.Phase == EBBBCharacterLifePhase::Alive && !Rescue.bHelping && !bRecovering
        && PreviousPhase == EBBBCharacterLifePhase::Alive && !bWasHelping && !bWasReceiving)
    {
        return;
    }
    if (!ensureMsgf(Desired, TEXT("[BBB][LifeMontage] 当前生命动作未配置 Phase=%d Helping=%d Receiving=%d"),
        int32(Life.Phase), Rescue.bHelping, Rescue.bReceiving))
    {
        return;
    }

    // 已批准的生命动作覆盖固定槽位 清理尚未执行的其他动作请求
    Context.AnimationInstance.ClearMontageContributions();
    auto &Requests = Context.AnimationMontageState;
    Requests.FullBodyMontageRequest = nullptr;
    Requests.bFullBodyMontageRequestPending = false;
    Requests.UpperBodyMontageRequest = nullptr;
    Requests.bUpperBodyMontageRequestPending = false;
    Requests.FullBodyAdditivePreAimMontageRequest = nullptr;
    Requests.bFullBodyAdditivePreAimMontageRequestPending = false;
    Requests.UpperBodyAdditiveMontageRequest = nullptr;
    Requests.bUpperBodyAdditiveMontageRequestPending = false;
    Requests.AdditiveHitReactMontageRequest = nullptr;
    Requests.bAdditiveHitReactMontageRequestPending = false;
    if (!Context.AnimationInstance.RegisterMontageContribution(BBBCharacterMontageSlots::FullBody, Desired))
    {
        return;
    }
    if (Desired == Config.ReceiveRescueMontage)
    {
        float CompleteTime = 0.0f;
        for (const FAnimNotifyEvent &Notify : Desired->Notifies)
        {
            if (Notify.NotifyStateClass && Notify.NotifyStateClass->IsA<UBBBCharacterRescueCompleteLogicAnimNotifyState>())
            {
                CompleteTime = Notify.GetTime() + Notify.GetDuration();
            }
        }
        if (ensureMsgf(CompleteTime > 0.0f, TEXT("[BBB][LifeMontage] 接受救援动作缺少完成逻辑通知 %s"), *Desired->GetPathName()))
        {
            Context.AnimationInstance.Montage_SetPlayRate(Desired, CompleteTime / FMath::Max(0.01f, Rescue.Duration));
        }
    }
}
