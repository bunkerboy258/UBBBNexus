#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterRescueTargetProcessor::Update(FBBBCharacterLifeUpdateContext &Context) const
{
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        return;
    }
    auto &Domain = Context.Data.Life;
    auto &State = Domain.RescueState;
    auto &Input = Domain.RescueInputState;
    auto &Delivery = Domain.RescueDeliveryState;
    const auto &Candidate = Domain.ReadRescueCandidateState();
    const auto &Life = Domain.ReadLifeState();
    const double Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    State.bCompletionPending = false;
    const auto Reply = [&](TWeakObjectPtr<APawn> Recipient, uint64 Operation, uint64 Round, bool bActive, FName Reason)
    {
        if (!Recipient.IsValid())
        {
            return;
        }
        Delivery.ReplyTargets.Add(Recipient);
        Delivery.ReplyOperations.Add(Operation);
        Delivery.ReplyRounds.Add(Round);
        Delivery.ReplyRevisions.Add(++Delivery.Serial);
        Delivery.ReplyActive.Add(bActive);
        Delivery.ReplyDurations.Add(FMath::Max(0.01f, Context.Config.RescueDuration));
        Delivery.ReplyReasons.Add(Reason);
    };
    const auto Stop = [&](FName Reason)
    {
        Reply(State.Partner, State.OperationId, State.DownedRevision, false, Reason);
        State.Partner.Reset();
        State.bReceiving = false;
        State.bAccepted = false;
        State.Progress = 0.0f;
        State.EndReason = Reason;
        ++State.Revision;
    };
    for (int32 Index = 0; Index < Input.CancelSources.Num(); ++Index)
    {
        uint64 &Last = State.RequestOperations.FindOrAdd(Input.CancelSources[Index]);
        Last = FMath::Max(Last, Input.CancelOperations[Index]);
        if (State.bReceiving && Input.CancelSources[Index] == State.Partner &&
            Input.CancelOperations[Index] == State.OperationId && Input.CancelRounds[Index] == State.DownedRevision)
        {
            Stop(TEXT("Cancelled"));
        }
    }
    Input.CancelSources.Reset();
    Input.CancelOperations.Reset();
    Input.CancelRounds.Reset();
    if (State.bReceiving)
    {
        const ABBBCharacter *Partner = Cast<ABBBCharacter>(State.Partner.Get());
        if (Life.Phase != EBBBCharacterLifePhase::Downed || Life.DownedRevision != State.DownedRevision ||
            !Partner || Partner->GetLifePhase() != EBBBCharacterLifePhase::Alive)
        {
            Stop(TEXT("PartnerUnavailable"));
        }
        if (State.bReceiving && (!Candidate.bCanRecover || !Candidate.EligiblePartners.Contains(State.Partner)))
        {
            Stop(TEXT("Blocked"));
        }
    }
    for (int32 Index = 0; Index < Input.RequestSources.Num(); ++Index)
    {
        const auto Source = Input.RequestSources[Index];
        const uint64 Operation = Input.RequestOperations[Index];
        const uint64 Round = Input.RequestRounds[Index];
        uint64 &Last = State.RequestOperations.FindOrAdd(Source);
        if (Operation == 0 || Operation <= Last)
        {
            continue;
        }
        Last = Operation;
        if (Life.Phase != EBBBCharacterLifePhase::Downed || Round != Life.DownedRevision ||
            !Candidate.bCanRecover || !Candidate.EligiblePartners.Contains(Source) || State.bHelping)
        {
            Reply(Source, Operation, Round, false, TEXT("Unavailable"));
            continue;
        }
        if (State.bReceiving)
        {
            Reply(Source, Operation, Round, false, TEXT("Occupied"));
            continue;
        }
        State.Partner = Source;
        State.OperationId = Operation;
        State.DownedRevision = Round;
        State.bReceiving = true;
        State.bAccepted = true;
        State.StartTime = Now;
        State.Duration = FMath::Max(0.01f, Context.Config.RescueDuration);
        State.Progress = 0.0f;
        State.EndReason = NAME_None;
        ++State.Revision;
        Reply(Source, Operation, Round, true, NAME_None);
    }
    Input.RequestSources.Reset();
    Input.RequestOperations.Reset();
    Input.RequestRounds.Reset();
    for (auto It = State.RequestOperations.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }
    if (State.bReceiving)
    {
        State.Progress = FMath::Clamp(float((Now - State.StartTime) / State.Duration), 0.0f, 1.0f);
        State.bCompletionPending = Now - State.StartTime >= State.Duration;
    }
}
