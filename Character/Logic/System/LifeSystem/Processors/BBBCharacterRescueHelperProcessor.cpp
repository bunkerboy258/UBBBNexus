#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueHelperProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterRescueHelperProcessor::Update(FBBBCharacterLifeUpdateContext &Context) const
{
    auto &Domain = Context.Data.Life;
    auto &State = Domain.RescueState;
    auto &Input = Domain.RescueInputState;
    auto &Delivery = Domain.RescueDeliveryState;
    Delivery.RequestTargets.Reset();
    Delivery.RequestOperations.Reset();
    Delivery.RequestRounds.Reset();
    Delivery.CancelTargets.Reset();
    Delivery.CancelOperations.Reset();
    Delivery.CancelRounds.Reset();
    Delivery.ReplyTargets.Reset();
    Delivery.ReplyOperations.Reset();
    Delivery.ReplyRounds.Reset();
    Delivery.ReplyRevisions.Reset();
    Delivery.ReplyActive.Reset();
    Delivery.ReplyDurations.Reset();
    Delivery.ReplyReasons.Reset();
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        return;
    }
    const auto &Life = Domain.ReadLifeState();
    const auto &Control = Context.Data.Parse.ReadControlState();
    const double Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    const auto Stop = [&](FName Reason, bool bNotify)
    {
        if (bNotify && State.Partner.IsValid())
        {
            Delivery.CancelTargets.Add(State.Partner);
            Delivery.CancelOperations.Add(State.OperationId);
            Delivery.CancelRounds.Add(State.DownedRevision);
            ++Delivery.Serial;
        }
        State.bHelping = false;
        State.bAccepted = false;
        State.Partner.Reset();
        State.Progress = 0.0f;
        State.EndReason = Reason;
        ++State.Revision;
    };
    for (int32 Index = 0; Index < Input.ReplySources.Num(); ++Index)
    {
        if (Input.ReplySources[Index] != State.Partner || Input.ReplyOperations[Index] != State.OperationId ||
            Input.ReplyRounds[Index] != State.DownedRevision || Input.ReplyRevisions[Index] <= State.ReplyRevision ||
            !State.bHelping)
        {
            continue;
        }
        State.ReplyRevision = Input.ReplyRevisions[Index];
        if (!Input.ReplyActive[Index])
        {
            Stop(Input.ReplyReasons[Index], false);
            continue;
        }
        State.bAccepted = true;
        State.StartTime = Now;
        State.Duration = FMath::Max(0.01f, Input.ReplyDurations[Index]);
        ++State.Revision;
    }
    Input.ReplySources.Reset();
    Input.ReplyOperations.Reset();
    Input.ReplyRounds.Reset();
    Input.ReplyRevisions.Reset();
    Input.ReplyActive.Reset();
    Input.ReplyDurations.Reset();
    Input.ReplyReasons.Reset();
    if (State.bHelping)
    {
        const ABBBCharacter *Partner = Cast<ABBBCharacter>(State.Partner.Get());
        FName Reason = NAME_None;
        if (Input.bCancel)
        {
            Reason = TEXT("Released");
        }
        if (!Control.MoveWorld.IsNearlyZero())
        {
            Reason = TEXT("Movement");
        }
        if (!Domain.ReadLifeInputState().Damages.IsEmpty())
        {
            Reason = TEXT("Damage");
        }
        if (Life.Phase != EBBBCharacterLifePhase::Alive || !Partner ||
            Partner->GetLifePhase() != EBBBCharacterLifePhase::Downed ||
            Partner->GetDownedRevision() != State.DownedRevision)
        {
            Reason = TEXT("PartnerUnavailable");
        }
        if (Partner && !Domain.ReadRescueCandidateState().EligiblePartners.Contains(State.Partner))
        {
            Reason = TEXT("Blocked");
        }
        if (Life.Phase == EBBBCharacterLifePhase::Alive && Partner && State.bAccepted &&
            Partner->GetLifePhase() == EBBBCharacterLifePhase::Alive &&
            Partner->GetDownedRevision() == State.DownedRevision)
        {
            Reason = TEXT("Completed");
        }
        if (!Reason.IsNone())
        {
            Stop(Reason, Reason != TEXT("Completed"));
        }
    }
    if (Input.bBegin && !Input.bCancel && !State.bHelping && !State.bReceiving &&
        Life.Phase == EBBBCharacterLifePhase::Alive && Control.MoveWorld.IsNearlyZero() &&
        Domain.ReadLifeInputState().Damages.IsEmpty())
    {
        ABBBCharacter *Target = Cast<ABBBCharacter>(Domain.ReadRescueCandidateState().Target.Get());
        State.EndReason = TEXT("NoTarget");
        if (Target && Target->GetDownedRevision() != 0)
        {
            State.Partner = Target;
            State.OperationId = ++State.LocalOperationCounter;
            State.DownedRevision = Target->GetDownedRevision();
            State.bHelping = true;
            State.bAccepted = false;
            State.ReplyRevision = 0;
            State.StartTime = Now;
            State.Duration = FMath::Max(0.01f, Context.Config.RescueDuration);
            State.EndReason = NAME_None;
            Delivery.RequestTargets.Add(Target);
            Delivery.RequestOperations.Add(State.OperationId);
            Delivery.RequestRounds.Add(State.DownedRevision);
            ++Delivery.Serial;
        }
        ++State.Revision;
    }
    State.Progress = State.bHelping && State.bAccepted
        ? FMath::Clamp(float((Now - State.StartTime) / State.Duration), 0.0f, 1.0f) : 0.0f;
    Input.bBegin = false;
    Input.bCancel = false;
}
