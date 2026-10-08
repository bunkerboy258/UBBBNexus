#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterLifeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/BBBCharacterConfig.h"

void FBBBCharacterLifeProcessor::Initialize(FBBBCharacterLifeUpdateContext &Context) const
{
    auto &State = Context.Data.Life.LifeState;
    if (!Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        State.Health = Context.Config.MaximumHealth;
        State.bInitialized = true;
        State.Revision = 1;
    }
}

void FBBBCharacterLifeProcessor::Update(FBBBCharacterLifeUpdateContext &Context) const
{
    auto &Domain = Context.Data.Life;
    auto &Input = Domain.LifeInputState;
    auto &State = Domain.LifeState;
    auto &Hit = Domain.HitState;
    auto &Delivery = Domain.DamageDeliveryState;
    const bool bMirror = Context.Data.External.ReadNetworkIdentityState().bIsMirror;
    const auto &World = Context.Data.External.ReadWorldState();
    Delivery.Damages.Reset();
    Delivery.Bones.Reset();
    Delivery.Positions.Reset();
    Delivery.Directions.Reset();
    Delivery.Sources.Reset();

    if (bMirror && Input.bHasResult && (!State.bInitialized || Input.ResultRevision > State.Revision))
    {
        const bool bFirstResult = !State.bInitialized;
        State.Phase = Input.ResultPhase;
        State.Health = Input.ResultHealth;
        State.Revision = Input.ResultRevision;
        State.DownedRevision = Input.ResultDownedRevision;
        State.bRecoveryCrouched = Input.bResultRecoveryCrouched;
        State.bInitialized = true;
        Hit.Serial = Input.ResultHitSerial;
        Hit.Bone = Input.ResultBone;
        Hit.Position = Input.ResultPosition;
        Hit.Direction = Input.ResultDirection;
        Hit.Time = bFirstResult ? World.WorldTimeSeconds - 1.0 : World.WorldTimeSeconds;
    }
    Input.bHasResult = false;

    if (!bMirror && !State.bInitialized)
    {
        Initialize(Context);
    }

    for (int32 Index = 0; Index < Input.Damages.Num(); ++Index)
    {
        if (bMirror)
        {
            Delivery.Damages.Add(Input.Damages[Index]);
            Delivery.Bones.Add(Input.Bones[Index]);
            Delivery.Positions.Add(Input.Positions[Index]);
            Delivery.Directions.Add(Input.Directions[Index]);
            Delivery.Sources.Add(Input.Sources[Index]);
            continue;
        }
        if (State.Phase == EBBBCharacterLifePhase::Dead)
        {
            continue;
        }

        State.Health = FMath::Max(0.0f, State.Health - Input.Damages[Index]);
        ++State.Revision;
        ++Hit.Serial;
        Hit.Bone = Input.Bones[Index];
        Hit.Position = Input.Positions[Index];
        Hit.Direction = Input.Directions[Index].GetSafeNormal();
        Hit.Time = World.WorldTimeSeconds;
        if (State.Health > 0.0f)
        {
            continue;
        }
        if (State.Phase == EBBBCharacterLifePhase::Alive)
        {
            State.Phase = EBBBCharacterLifePhase::Downed;
            State.Health = Context.Config.DownedHealth;
            State.DownedRevision = State.Revision;
            State.bRecoveryCrouched = false;
            continue;
        }
        State.Phase = EBBBCharacterLifePhase::Dead;
    }

    auto &Rescue = Domain.RescueState;
    if (!bMirror && Rescue.bReceiving && (Rescue.bCompletionPending || State.Phase != EBBBCharacterLifePhase::Downed))
    {
        const bool bSuccess = Rescue.bCompletionPending && State.Phase == EBBBCharacterLifePhase::Downed &&
            State.DownedRevision == Rescue.DownedRevision && Domain.ReadRescueCandidateState().bCanRecover;
        FName Reason = TEXT("PartnerUnavailable");
        if (bSuccess)
        {
            State.Phase = EBBBCharacterLifePhase::Alive;
            State.Health = FMath::Clamp(Context.Config.RescueHealth, 1.0f, Context.Config.MaximumHealth);
            State.bRecoveryCrouched = Domain.ReadRescueCandidateState().bRecoverCrouched;
            ++State.Revision;
            Reason = TEXT("Completed");
        }
        auto &Results = Domain.RescueDeliveryState;
        if (Rescue.Partner.IsValid())
        {
            Results.ReplyTargets.Add(Rescue.Partner);
            Results.ReplyOperations.Add(Rescue.OperationId);
            Results.ReplyRounds.Add(Rescue.DownedRevision);
            Results.ReplyRevisions.Add(++Results.Serial);
            Results.ReplyActive.Add(false);
            Results.ReplyDurations.Add(Rescue.Duration);
            Results.ReplyReasons.Add(Reason);
        }
        Rescue.bReceiving = false;
        Rescue.bAccepted = false;
        Rescue.bCompletionPending = false;
        Rescue.Partner.Reset();
        Rescue.Progress = 0.0f;
        Rescue.EndReason = Reason;
        ++Rescue.Revision;
    }
    State.bActionsAllowed = State.Phase == EBBBCharacterLifePhase::Alive && !Rescue.bHelping && !Rescue.bReceiving;

    if (!Delivery.Damages.IsEmpty())
    {
        ++Delivery.Serial;
    }
    Input.Damages.Reset();
    Input.Bones.Reset();
    Input.Positions.Reset();
    Input.Directions.Reset();
    Input.Sources.Reset();
}
