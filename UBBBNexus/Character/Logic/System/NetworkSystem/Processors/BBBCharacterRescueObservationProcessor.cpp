#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterRescueObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueRequestLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueReplyLocalControlPacket.h"

void FBBBCharacterRescueObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    ABBBCharacter *Sender = Cast<ABBBCharacter>(Context.NetworkComponent.GetOwner());
    if (!Sender)
    {
        return;
    }
    auto &Observation = Context.Data.Network.RescueObservationState;
    auto &Inbox = Context.Data.Network.RescueInboxState;
    const auto &Delivery = Context.Data.Life.ReadRescueDeliveryState();
    const auto Request = [&](ABBBCharacter *Target, APawn *Source, uint64 Operation, uint64 Round)
    {
        if (!Target || !Source || Target->GetWorld() != Sender->GetWorld() || Source->GetWorld() != Sender->GetWorld())
        {
            return;
        }
        if (Target->IsLocallyControlled())
        {
            Target->SubmitInput(FBBBCharacterRescueRequestLocalControlPacket{{Source}, {Operation}, {Round}});
            return;
        }
        if (Context.NetworkIdentityState.bHasAuthority)
        {
            Target->GetCharacterNetworkComponent()->ClientRequestRescue(Source, Operation, Round);
            return;
        }
        Context.NetworkComponent.ServerRequestRescue(Target, Operation, Round);
    };
    const auto End = [&](ABBBCharacter *Target, APawn *Source, uint64 Operation, uint64 Round)
    {
        if (!Target || !Source || Target->GetWorld() != Sender->GetWorld() || Source->GetWorld() != Sender->GetWorld())
        {
            return;
        }
        if (Target->IsLocallyControlled())
        {
            Target->SubmitInput(FBBBCharacterRescueEndLocalControlPacket{{Source}, {Operation}, {Round}});
            return;
        }
        if (Context.NetworkIdentityState.bHasAuthority)
        {
            Target->GetCharacterNetworkComponent()->ClientEndRescue(Source, Operation, Round);
            return;
        }
        Context.NetworkComponent.ServerEndRescue(Target, Operation, Round);
    };
    const auto Reply = [&](ABBBCharacter *Target, APawn *Source, uint64 Operation, uint64 Round, uint64 Revision, bool bActive, float Duration, FName Reason)
    {
        if (!Target || !Source || Target->GetWorld() != Sender->GetWorld() || Source->GetWorld() != Sender->GetWorld())
        {
            return;
        }
        if (Target->IsLocallyControlled())
        {
            Target->SubmitInput(FBBBCharacterRescueReplyLocalControlPacket{{Source}, {Operation}, {Round}, {Revision}, {bActive}, {Duration}, {Reason}});
            return;
        }
        if (Context.NetworkIdentityState.bHasAuthority)
        {
            Target->GetCharacterNetworkComponent()->ClientReplyRescue(Source, Operation, Round, Revision, bActive, Duration, Reason);
            return;
        }
        Context.NetworkComponent.ServerReplyRescue(Target, Operation, Round, Revision, bActive, Duration, Reason);
    };
    if (Delivery.Serial > Observation.DeliverySerial)
    {
        for (int32 Index = 0; Index < Delivery.RequestTargets.Num(); ++Index)
        {
            Request(Cast<ABBBCharacter>(Delivery.RequestTargets[Index].Get()), Sender, Delivery.RequestOperations[Index], Delivery.RequestRounds[Index]);
        }
        for (int32 Index = 0; Index < Delivery.CancelTargets.Num(); ++Index)
        {
            End(Cast<ABBBCharacter>(Delivery.CancelTargets[Index].Get()), Sender, Delivery.CancelOperations[Index], Delivery.CancelRounds[Index]);
        }
        for (int32 Index = 0; Index < Delivery.ReplyTargets.Num(); ++Index)
        {
            Reply(Cast<ABBBCharacter>(Delivery.ReplyTargets[Index].Get()), Sender, Delivery.ReplyOperations[Index], Delivery.ReplyRounds[Index], Delivery.ReplyRevisions[Index], Delivery.ReplyActive[Index], Delivery.ReplyDurations[Index], Delivery.ReplyReasons[Index]);
        }
        Observation.DeliverySerial = Delivery.Serial;
    }
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        for (int32 Index = 0; Index < Inbox.RequestTargets.Num(); ++Index)
        {
            Request(Cast<ABBBCharacter>(Inbox.RequestTargets[Index].Get()), Inbox.RequestSources[Index].Get(), Inbox.RequestOperations[Index], Inbox.RequestRounds[Index]);
        }
        for (int32 Index = 0; Index < Inbox.CancelTargets.Num(); ++Index)
        {
            End(Cast<ABBBCharacter>(Inbox.CancelTargets[Index].Get()), Inbox.CancelSources[Index].Get(), Inbox.CancelOperations[Index], Inbox.CancelRounds[Index]);
        }
        for (int32 Index = 0; Index < Inbox.ReplyTargets.Num(); ++Index)
        {
            Reply(Cast<ABBBCharacter>(Inbox.ReplyTargets[Index].Get()), Inbox.ReplySources[Index].Get(), Inbox.ReplyOperations[Index], Inbox.ReplyRounds[Index], Inbox.ReplyRevisions[Index], Inbox.ReplyActive[Index], Inbox.ReplyDurations[Index], Inbox.ReplyReasons[Index]);
        }
    }
    Inbox.RequestSources.Reset();
    Inbox.RequestTargets.Reset();
    Inbox.RequestOperations.Reset();
    Inbox.RequestRounds.Reset();
    Inbox.CancelSources.Reset();
    Inbox.CancelTargets.Reset();
    Inbox.CancelOperations.Reset();
    Inbox.CancelRounds.Reset();
    Inbox.ReplySources.Reset();
    Inbox.ReplyTargets.Reset();
    Inbox.ReplyOperations.Reset();
    Inbox.ReplyRounds.Reset();
    Inbox.ReplyRevisions.Reset();
    Inbox.ReplyActive.Reset();
    Inbox.ReplyDurations.Reset();
    Inbox.ReplyReasons.Reset();
    const auto &State = Context.Data.Life.ReadRescueState();
    if (State.Revision <= Observation.Revision)
    {
        return;
    }
    const float Elapsed = State.bHelping || State.bReceiving
        ? FMath::Max(0.0f, float(Context.WorldState.WorldTimeSeconds - State.StartTime)) : 0.0f;
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateRescue(State.Partner.Get(), State.OperationId, State.DownedRevision,
            State.Revision, State.bHelping, State.bReceiving, State.bAccepted, Elapsed, State.Duration, State.EndReason);
        Observation.Revision = State.Revision;
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitRescue(State.Partner.Get(), State.OperationId, State.DownedRevision,
            State.Revision, State.bHelping, State.bReceiving, State.bAccepted, Elapsed, State.Duration, State.EndReason);
        Observation.Revision = State.Revision;
    }
}
