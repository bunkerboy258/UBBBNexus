#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterLifeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Engine/World.h"

void FBBBCharacterLifeProcessor::Initialize(FBBBCharacterLifeUpdateContext &Context) const
{
    auto &State = Context.Data.Life.LifeState;
    if (!Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        State.Health = Context.Config.MaximumHealth;
        State.Armor = FMath::Max(0.0f, Context.Config.MaximumArmor);
        State.SecondsSinceDamage = FMath::Max(0.0f, Context.Config.ArmorRegenDelaySeconds);
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
    Delivery.Hits.Reset();

    if (bMirror && Input.bHasResult && (!State.bInitialized || Input.ResultRevision > State.Revision))
    {
        const bool bFirstResult = !State.bInitialized;
        State.Phase = Input.ResultPhase;
        State.Health = Input.ResultHealth;
        State.Armor = Input.ResultPhase == EBBBCharacterLifePhase::Alive ? FMath::Max(0.0f, Input.ResultArmor) : 0.0f;
        State.MovementSpeedScale = Input.ResultMovementSpeedScale;
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
    if (!bMirror && Input.bHasEnvironmentResult
        && !FMath::IsNearlyEqual(State.MovementSpeedScale, Input.EnvironmentSpeedScale))
    {
        State.MovementSpeedScale = Input.EnvironmentSpeedScale;
        ++State.Revision;
    }
    Input.bHasEnvironmentResult = false;

    if (!bMirror && !State.bInitialized)
    {
        Initialize(Context);
    }

    if (!bMirror && State.Phase == EBBBCharacterLifePhase::Alive)
    {
        for (float Recovery : Input.Recoveries)
        {
            State.Health = FMath::Min(Context.Config.MaximumHealth, State.Health + Recovery);
            ++State.Revision;
        }
    }
    Input.Recoveries.Reset();
    bool bFirstImpact = true;
    bool bTookDamage = false;
    for (const auto& Value : Input.Hits)
    {
        if (bMirror)
        {
            Delivery.Hits.Add(Value);
            continue;
        }
        if (State.Phase == EBBBCharacterLifePhase::Dead)
        {
            continue;
        }

        if (Value.AddDamage > 0.0f)
        {
            bTookDamage = true;
            State.SecondsSinceDamage = 0.0f;
            if (State.Phase == EBBBCharacterLifePhase::Alive)
            {
                const float Absorbed = FMath::Min(State.Armor, Value.AddDamage);
                State.Armor = FMath::Max(0.0f, State.Armor - Absorbed);
                const float HealthDamage = FMath::Max(0.0f, Value.AddDamage - Absorbed);
                State.Health = FMath::Max(0.0f, State.Health - HealthDamage);
            }
            else
            {
                State.Armor = 0.0f;
                State.Health = FMath::Max(0.0f, State.Health - Value.AddDamage);
            }
        }
        ++State.Revision;
        ++Hit.Serial;
        Hit.Bone = Value.Bone;
        Hit.Position = Value.Position;
        Hit.Direction = Value.Direction.GetSafeNormal();
        Hit.Time = World.WorldTimeSeconds;
        const bool bHadExplicitImpact = Value.AddHoldSeconds > 0 || !Value.AddImpulse.IsNearlyZero() || !Value.AddVelocity.IsNearlyZero();
        if (bHadExplicitImpact)
        {
            if (bFirstImpact)
            {
                Hit.ImpactVelocity = FVector::ZeroVector;
                Hit.ImpactImpulse = FVector::ZeroVector;
                Hit.ImpactHoldSeconds = 0;
                bFirstImpact = false;
            }
            Hit.ImpactVelocity += Value.AddVelocity;
            Hit.ImpactImpulse += Value.AddImpulse;
            Hit.ImpactHoldSeconds = FMath::Max(Hit.ImpactHoldSeconds, FMath::Min(Value.AddHoldSeconds, Context.Config.MaximumImpactHoldSeconds));
            ++Hit.ImpactSerial;
        }
        if (State.Health > 0.0f)
        {
            continue;
        }
        const bool bTransitionToDowned = State.Phase == EBBBCharacterLifePhase::Alive;
        if (bTransitionToDowned)
        {
            State.Phase = EBBBCharacterLifePhase::Downed;
            State.Health = Context.Config.DownedHealth;
            State.Armor = 0.0f;
            State.DownedRevision = State.Revision;
            State.bRecoveryCrouched = false;
        }
        else
        {
            State.Phase = EBBBCharacterLifePhase::Dead;
            State.Armor = 0.0f;
        }

        // 致死或首次击倒时补足最低水平推力与抬升 避免原地软倒
        const FVector KnockDir2D = Hit.Direction.GetSafeNormal2D();
        if (!KnockDir2D.IsNearlyZero() && (Context.Config.LethalKnockbackSpeed > 0.0f || Context.Config.LethalKnockbackLift > 0.0f))
        {
            if (bFirstImpact)
            {
                Hit.ImpactVelocity = FVector::ZeroVector;
                Hit.ImpactImpulse = FVector::ZeroVector;
                Hit.ImpactHoldSeconds = 0;
                bFirstImpact = false;
            }
            const float CurrentForwardSpeed = FVector::DotProduct(Hit.ImpactVelocity, KnockDir2D);
            if (CurrentForwardSpeed < Context.Config.LethalKnockbackSpeed)
            {
                Hit.ImpactVelocity += KnockDir2D * (Context.Config.LethalKnockbackSpeed - CurrentForwardSpeed);
            }
            Hit.ImpactVelocity.Z = FMath::Max(Hit.ImpactVelocity.Z, Context.Config.LethalKnockbackLift);
            if (bTransitionToDowned && Context.Config.DownedImpactHoldSeconds > 0.0f)
            {
                Hit.ImpactHoldSeconds = FMath::Max(Hit.ImpactHoldSeconds,
                    FMath::Min(Context.Config.DownedImpactHoldSeconds, Context.Config.MaximumImpactHoldSeconds));
            }
            if (!bHadExplicitImpact)
            {
                ++Hit.ImpactSerial;
            }
        }
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
            State.Armor = 0.0f;
            State.SecondsSinceDamage = 0.0f;
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

    // 脱战超过延迟后缓慢自然回复护甲
    if (!bMirror && State.Phase == EBBBCharacterLifePhase::Alive)
    {
        const float MaxArmor = FMath::Max(0.0f, Context.Config.MaximumArmor);
        if (State.Armor > MaxArmor)
        {
            State.Armor = MaxArmor;
            ++State.Revision;
        }
        if (!bTookDamage && MaxArmor > 0.0f && State.Armor < MaxArmor)
        {
            const float DeltaSeconds = FMath::Max(World.FrameDeltaSeconds, 0.0f);
            State.SecondsSinceDamage += DeltaSeconds;
            if (State.SecondsSinceDamage >= FMath::Max(0.0f, Context.Config.ArmorRegenDelaySeconds))
            {
                const float RegenRate = FMath::Max(0.0f, Context.Config.ArmorRegenPerSecond);
                const float NextArmor = FMath::Min(MaxArmor, State.Armor + RegenRate * DeltaSeconds);
                if (!FMath::IsNearlyEqual(NextArmor, State.Armor))
                {
                    State.Armor = NextArmor;
                    ++State.Revision;
                }
            }
        }
    }
    const auto &Physical = Context.Data.PhysicalPresentation.ReadPhysicalPresentationState();
    State.bActionsAllowed = State.Phase == EBBBCharacterLifePhase::Alive && !Rescue.bHelping && !Rescue.bReceiving
        && !Physical.bRagdollRequested && Physical.RagdollSnapshot.Phase == EBBBCharacterPhysicalPhase::Animated;
    Context.Character.SetCanBeDamaged(State.bInitialized && State.Phase != EBBBCharacterLifePhase::Dead
        && !Context.Character.GetWorld()->IsPlayingReplay());

    if (!Delivery.Hits.IsEmpty())
    {
        ++Delivery.Serial;
    }
    Input.Hits.Reset();
}
