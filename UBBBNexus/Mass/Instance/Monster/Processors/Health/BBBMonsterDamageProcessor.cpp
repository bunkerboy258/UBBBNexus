#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "MassEntitySubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"

#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"

namespace
{
    /** 把本批命中解释为僵尸自身的部位与行动结果 */
    void ResolveHits(UWorld& World, FMassEntityHandle Entity, const UBBBMonsterDefinition& Definition,
        const TArray<FBBBHitContribution>& Hits, TMap<int64, FBBBMonsterDamageTotals>& Ledger, double Time)
    {
        TMap<TPair<int64, FGuid>, FBBBMonsterStaggerContribution> Candidates;
        auto* Mass = World.GetSubsystem<UBBBMassSubsystem>();
        float Weight = 0.f;
        for (uint8 Part = 0; Part <= 5; ++Part)
        {
            Weight += Definition.GetBodyPart(static_cast<EBBBMonsterHitRegion>(Part)).RadialWeight;
        }
        TSet<int64> Started;
        for (const auto& Hit : Hits)
        {
            if (!Hit.IsValid() || (!Hit.bRadial && Hit.Part > 5) || (Hit.bRadial && Weight <= 0))
            {
                UE_LOG(LogTemp, Warning, TEXT("[BBBHit] 拒绝无效僵尸命中 Entity=%d Part=%d"), Entity.Index, Hit.Part);
                continue;
            }
            FBBBMonsterDamageTotals Identity;
            Identity.PlayerId = Hit.PlayerId;
            Identity.bWorldSource = Hit.bWorldSource;
            auto& Value = Ledger.FindOrAdd(Identity.WriterKey());
            Value.PlayerId = Hit.PlayerId;
            Value.bWorldSource = Hit.bWorldSource;
            Value.LastHitTime = Time;
            const auto ContactRegion = Hit.Part <= 5 ? static_cast<EBBBMonsterHitRegion>(Hit.Part) : EBBBMonsterHitRegion::Torso;
            Value.LastHitRegion = ContactRegion;
            auto& Result = Value.LastHitResult;
            if (!Started.Contains(Identity.WriterKey()))
            {
                Result.Impulse = FVector::ZeroVector;
                Result.Velocity = FVector::ZeroVector;
                Started.Add(Identity.WriterKey());
            }
            ++Result.Sequence;
            Result.Time = Time;
            Result.Region = ContactRegion;
            Result.Position = Hit.Position;
            const FVector PhysicalDirection = Hit.AddVelocity.IsNearlyZero() ? Hit.AddImpulse : Hit.AddVelocity;
            Result.Direction = Hit.Direction.GetSafeNormal(UE_SMALL_NUMBER, PhysicalDirection.GetSafeNormal());
            Result.Normal = Hit.Normal.GetSafeNormal(UE_SMALL_NUMBER, -Result.Direction);
            Result.Impulse += Hit.AddImpulse;
            Result.Velocity += Hit.AddVelocity;
            Value.bHasSourcePosition = Hit.bHasSourcePosition;
            Value.LastSourcePosition = Hit.SourcePosition;
            for (uint8 Part = 0; Part <= 5; ++Part)
            {
                if (!Hit.bRadial && Part != Hit.Part)
                {
                    continue;
                }
                const auto Region = static_cast<EBBBMonsterHitRegion>(Part);
                const auto& Rule = Definition.GetBodyPart(Region);
                const float Share = Hit.bRadial ? Rule.RadialWeight / Weight : 1.0f;
                const double Amount = Share * (Hit.AddDamage * (1.0 - Rule.Durability) + Hit.AddDurableDamage * Rule.Durability);
                Value.Parts.Add(Region, Amount);
                if (Part == 0 && Mass && Hit.AddWound > 0 && Amount > 0)
                {
                    Mass->QueryMonsterDestruction(Entity, Hit.Position, Hit.Direction, Hit.AddWound, Value.Destruction);
                }
                if (!Hit.AttackId.IsValid() || (Value.Stagger.AttackId == Hit.AttackId && Value.Stagger.bEvaluated)
                    || Hit.AttackTime < Value.Stagger.AttackTime)
                {
                    continue;
                }
                auto& Candidate = Candidates.FindOrAdd({Identity.WriterKey(), Hit.AttackId});
                Candidate.AttackId = Hit.AttackId;
                Candidate.AttackTime = Hit.AttackTime;
                Candidate.HitTime = Time;
                const uint32 Seed = HashCombineFast(GetTypeHash(Hit.AttackId),
                    HashCombineFast(GetTypeHash(Entity.Index), GetTypeHash(Entity.SerialNumber)));
                Candidate.Roll = float(Seed >> 8) / 16777216.0f;
                const float Force = Hit.AddStagger * Rule.StaggerMultiplier;
                if (Force > Candidate.Impact || (Force == Candidate.Impact && Region > Candidate.Region))
                {
                    Candidate.Impact = Force;
                    Candidate.Region = Region;
                    Candidate.Chance = FMath::Clamp((Force - Definition.StaggerStartValue)
                        / FMath::Max(UE_SMALL_NUMBER, Definition.StaggerFullValue - Definition.StaggerStartValue), 0.f, 1.f);
                    Candidate.Duration = Rule.StaggerDuration;
                }
            }
        }
        double Available = 0;
        for (const auto& Entry : Ledger)
        {
            if (Entry.Value.Stagger.ActiveAttackId.IsValid())
            {
                Available = FMath::Max(Available, Entry.Value.Stagger.EndsAt + FBBBMonsterStaggerContribution::RecoveryDuration);
            }
        }
        Candidates.ValueSort([](const auto& A, const auto& B)
        {
            return A.AttackTime < B.AttackTime || (A.AttackTime == B.AttackTime && A.AttackId < B.AttackId);
        });
        for (auto& Entry : Candidates)
        {
            auto& Candidate = Entry.Value;
            if (Candidate.Resolve(Available))
            {
                Candidate.ActiveAttackId = Candidate.AttackId;
                Candidate.StartedAt = Time;
                Candidate.EndsAt = Time + Candidate.Duration;
                Candidate.ActiveRegion = Candidate.Region;
                Available = Candidate.EndsAt + FBBBMonsterStaggerContribution::RecoveryDuration;
            }
            Ledger.FindChecked(Entry.Key.Key).Stagger.Merge(Candidate);
        }
    }
}

bool UBBBMonsterDamageProcessor::Query(UWorld& World, FMassEntityHandle Entity,
    TArray<FBBBMonsterDamageTotals>& Result)
{
    Result.Reset();
    auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!IsInGameThread() || Entities == nullptr)
    {
        return false;
    }
    const auto& Manager = Entities->GetEntityManager();
    if (!Manager.IsEntityValid(Entity))
    {
        return false;
    }
    const auto* Damage = Manager.GetFragmentDataPtr<FBBBMonsterDamageFragment>(Entity);
    const auto* Input = Manager.GetFragmentDataPtr<FBBBMonsterHealthInputFragment>(Entity);
    const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
    if (Damage == nullptr || Input == nullptr || Network == nullptr)
    {
        return false;
    }
    FBBBMonsterDamageTotalsRemoteMessagePacket Snapshot;
    for (const auto& Value : Damage->Contributions)
    {
        Snapshot.Include(Value.Value);
    }
    const auto* NetworkInput = Manager.GetFragmentDataPtr<FBBBMonsterNetworkInputFragment>(Entity);
    const FGuid EffectiveId = Network->InstanceId.IsValid() ? Network->InstanceId
        : (NetworkInput != nullptr && NetworkInput->State.bActive && NetworkInput->State.Packet.IsValid()
            ? NetworkInput->State.Packet.InstanceId : FGuid());
    if (Input->RemoteDamage.bActive && Input->RemoteDamage.Packet.IsValid()
        && Input->RemoteDamage.Packet.InstanceId == EffectiveId)
    {
        for (const auto& Value : Input->RemoteDamage.Packet.Contributions)
        {
            Snapshot.Include(Value);
        }
    }
    TMap<int64, FBBBMonsterDamageTotals> Ledger;
    for (const auto& Value : Snapshot.Contributions)
    {
        Ledger.Add(Value.WriterKey(), Value);
    }
    if (Network->Definition.IsValid())
    {
        auto Hits = Damage->PendingHits;
        if (Input->Damage.bActive && Input->Damage.Packet.IsValid())
        {
            Hits.Append(Input->Damage.Packet.Hits);
        }
        const auto* Game = World.GetGameState();
        ResolveHits(World, Entity, *Network->Definition, Hits, Ledger, Game ? Game->GetServerWorldTimeSeconds() : World.GetTimeSeconds());
    }
    Ledger.GenerateValueArray(Result);
    Result.Sort([](const auto& A, const auto& B)
    {
        return A.WriterKey() < B.WriterKey();
    });
    return true;
}

UBBBMonsterDamageProcessor::UBBBMonsterDamageProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Parse);
}

bool UBBBMonsterDamageProcessor::QueryPart(UWorld& World, FMassEntityHandle Entity, uint8 Part, FBBBMonsterBodyPartDefinition& Result)
{
    const auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!IsInGameThread() || Entities == nullptr || Part > static_cast<uint8>(EBBBMonsterHitRegion::RightLeg))
    {
        return false;
    }
    const auto& Manager = Entities->GetEntityManager();
    if (!Manager.IsEntityValid(Entity))
    {
        return false;
    }
    const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
    if (Network == nullptr || Network->Definition == nullptr)
    {
        return false;
    }
    Result = Network->Definition->GetBodyPart(static_cast<EBBBMonsterHitRegion>(Part));
    return Result.IsValid();
}

void UBBBMonsterDamageProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FBBBMonsterDamageFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterHitReactionFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
    EntityQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterDamageProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    const float Now = World->GetTimeSeconds();
    const AGameStateBase* GameState = World->GetGameState();
    const double ServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : Now;
    EntityQuery.ForEachEntityChunk(Context, [Now, ServerTime, World](FMassExecutionContext& Chunk)
    {
        auto Damage = Chunk.GetMutableFragmentView<FBBBMonsterDamageFragment>();
        auto Health = Chunk.GetMutableFragmentView<FBBBMonsterHealthFragment>();
        auto Mobility = Chunk.GetMutableFragmentView<FBBBMonsterMobilityFragment>();
        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Pending = Damage[Index].PendingHits;
            if (!Pending.IsEmpty() && Network[Index].Definition.IsValid())
            {
                ResolveHits(*World, Chunk.GetEntity(Index), *Network[Index].Definition, Pending, Damage[Index].Contributions, ServerTime);
                Pending.Reset();
            }
            auto Reactions = Chunk.GetMutableFragmentView<FBBBMonsterHitReactionFragment>();
            const FBBBMonsterHitResult* LatestResponse = nullptr;
            int64 LatestWriter = MIN_int64;
            FVector Impulse = FVector::ZeroVector;
            FVector Velocity = FVector::ZeroVector;
            for (const auto& Entry : Damage[Index].Contributions)
            {
                const auto& Result = Entry.Value.LastHitResult;
                auto& Presented = Damage[Index].PresentedHitSequences.FindOrAdd(Entry.Key);
                if (Result.Sequence <= Presented)
                {
                    continue;
                }
                Presented = Result.Sequence;
                const double Age = ServerTime - Result.Time;
                if (Age < -0.1 || Age > 0.3 || !Result.IsValid())
                {
                    continue;
                }
                Impulse += Result.Impulse;
                Velocity += Result.Velocity;
                if (!LatestResponse || Result.Time > LatestResponse->Time
                    || (Result.Time == LatestResponse->Time && Entry.Key > LatestWriter))
                {
                    LatestResponse = &Result;
                    LatestWriter = Entry.Key;
                }
            }
            if (LatestResponse && !Reactions.IsEmpty())
            {
                auto& Hit = Reactions[Index];
                Hit.Region = LatestResponse->Region;
                Hit.Position = LatestResponse->Position;
                Hit.Direction = LatestResponse->Direction;
                Hit.Normal = LatestResponse->Normal;
                Hit.ImpactImpulse = Impulse;
                Hit.ImpactVelocity = Velocity;
                Hit.Age = FMath::Max(0.f, float(ServerTime - LatestResponse->Time));
                ++Hit.Serial;
            }
            FBBBMonsterPartDamage Totals;
            const FBBBMonsterDamageTotals* Latest = nullptr;
            const FBBBMonsterStaggerContribution* ActiveStagger = nullptr;
            for (const auto& Value : Damage[Index].Contributions)
            {
                for (uint8 Part = 0; Part <= static_cast<uint8>(EBBBMonsterHitRegion::RightLeg); ++Part)
                {
                    const auto Region = static_cast<EBBBMonsterHitRegion>(Part);
                    Totals.Add(Region, Value.Value.Parts.Get(Region));
                }
                if (!Latest || Value.Value.LastHitTime > Latest->LastHitTime
                    || (Value.Value.LastHitTime == Latest->LastHitTime && Value.Value.PlayerId > Latest->PlayerId))
                {
                    Latest = &Value.Value;
                }
                const auto& Stagger = Value.Value.Stagger;
                if (Stagger.ActiveAttackId.IsValid()
                    && (ActiveStagger == nullptr || Stagger.StartedAt > ActiveStagger->StartedAt
                        || (Stagger.StartedAt == ActiveStagger->StartedAt && ActiveStagger->ActiveAttackId < Stagger.ActiveAttackId)))
                {
                    ActiveStagger = &Stagger;
                }
            }
            auto& Motion = Mobility[Index];
            const auto* Settings = Network[Index].Definition.Get();
            if (!Settings)
            {
                continue;
            }
            double MainDamage = 0.0;
            uint8 Destroyed = 0;
            bool bFatal = false;
            FBBBMonsterPartDamage Ratios;
            for (uint8 Part = 0; Part <= static_cast<uint8>(EBBBMonsterHitRegion::RightLeg); ++Part)
            {
                const auto Region = static_cast<EBBBMonsterHitRegion>(Part);
                const auto& Definition = Settings->GetBodyPart(Region);
                const double Amount = Totals.Get(Region);
                Ratios.Add(Region, FMath::Clamp(Amount / (Health[Index].MaxHealth * Definition.HealthFraction), 0.0, 1.0));
                MainDamage += Amount * Definition.MainTransfer;
                if (Amount + UE_SMALL_NUMBER >= Health[Index].MaxHealth * Definition.HealthFraction)
                {
                    Destroyed |= 1 << Part;
                    bFatal |= Definition.bFatal;
                }
            }
            const double Added = FMath::Max(0.0, Totals.Sum() - Health[Index].ResolvedDamage);
            Health[Index].MainHealth = static_cast<float>(FMath::Max(0.0, Health[Index].MaxHealth - MainDamage));
            Health[Index].CurrentHealth = bFatal ? 0.0f : Health[Index].MainHealth;
            Health[Index].DestroyedParts = Destroyed;
            Health[Index].PartDamageRatios = Ratios;
            Health[Index].ResolvedDamage = Totals.Sum();
            Damage[Index].bReceivedDamage = Added > 0.0;
            const uint8 ArmMask = (1 << static_cast<uint8>(EBBBMonsterHitRegion::LeftArm))
                | (1 << static_cast<uint8>(EBBBMonsterHitRegion::RightArm));
            const uint8 Arms = Destroyed & ArmMask;
            Motion.AttackRatio = Arms == 0 ? 1.0f : Settings->OneArmAttackRatio;
            if (Arms == ArmMask)
            {
                Motion.AttackRatio = 0.0f;
            }
            if (Health[Index].CurrentHealth <= 0.0f)
            {
                Motion.SlowMinimumRatio = 1.0f;
                Motion.SlowRecoveryStartedAt = Now;
                Motion.SlowEndsAt = Now;
                Motion.HitStopEndsAt = Now;
                Motion.StaggerEndsAt = Now;
                Motion.StaggerStartedAt = Now;
                continue;
            }
            const uint8 LegMask = (1 << static_cast<uint8>(EBBBMonsterHitRegion::LeftLeg))
                | (1 << static_cast<uint8>(EBBBMonsterHitRegion::RightLeg));
            if (!Motion.bCrawling && (Destroyed & LegMask) != 0)
            {
                Motion.bCrawling = true;
                Motion.CrawlStartedAt = Network[Index].bHasCrawlTime
                    ? FMath::Min(Now, Network[Index].CrawlStartedAt) : Now;
                Motion.StaggerStartedAt = Now;
                Motion.StaggerEndsAt = Now;
                Motion.HitStopEndsAt = Now;
                UE_LOG(LogTemp, Log, TEXT("[BBBMonsterCrawl] Entity=%d Destroyed=%d"),
                    Chunk.GetEntity(Index).Index, Destroyed);
            }
            if (!Motion.bCrawling && ActiveStagger != nullptr && ActiveStagger->ActiveAttackId != Motion.StaggerAttackId)
            {
                const bool bMayStart = !Motion.IsStaggering(Now)
                    && (Motion.StaggerEndsAt <= Motion.StaggerStartedAt
                        || Now + ActiveStagger->StartedAt - ServerTime >= Motion.StaggerEndsAt + FBBBMonsterStaggerContribution::RecoveryDuration);
                if (ServerTime >= ActiveStagger->StartedAt - 0.1)
                {
                    Motion.StaggerAttackId = ActiveStagger->ActiveAttackId;
                    if (bMayStart && ServerTime < ActiveStagger->EndsAt)
                    {
                        Motion.StaggerStartedAt = Now + static_cast<float>(ActiveStagger->StartedAt - ServerTime);
                        Motion.StaggerEndsAt = Now + static_cast<float>(ActiveStagger->EndsAt - ServerTime);
                        Motion.StaggerRegion = ActiveStagger->ActiveRegion;
                        Motion.HitStopEndsAt = Motion.StaggerEndsAt;
                        UE_LOG(LogTemp, Verbose, TEXT("[BBBMonsterStagger] Entity=%d Region=%d Start=%.3f End=%.3f"),
                            Chunk.GetEntity(Index).Index, static_cast<int32>(Motion.StaggerRegion),
                            Motion.StaggerStartedAt, Motion.StaggerEndsAt);
                    }
                }
            }
            if (!Damage[Index].bReceivedDamage || !Latest)
            {
                continue;
            }
            float Ratio = Settings->BodyHitSpeedRatio;
            float Duration = Settings->BodyHitSlowDuration;
            if (Latest->LastHitRegion == EBBBMonsterHitRegion::LeftArm || Latest->LastHitRegion == EBBBMonsterHitRegion::RightArm)
            {
                Ratio = Settings->ArmHitSpeedRatio;
                Duration = Settings->ArmHitSlowDuration;
            }
            if (Latest->LastHitRegion == EBBBMonsterHitRegion::LeftLeg || Latest->LastHitRegion == EBBBMonsterHitRegion::RightLeg)
            {
                Ratio = Settings->LegHitSpeedRatio;
                Duration = Settings->LegHitSlowDuration;
            }
            const double Age = ServerTime - Latest->LastHitTime;
            const float HoldDuration = Settings->HitSlowHoldDuration;
            if (Age < -0.1 || Age >= HoldDuration + Duration)
            {
                continue;
            }
            const float HitTime = Now - static_cast<float>(FMath::Max(Age, 0.0));
            Motion.SlowMinimumRatio = FMath::Min(Motion.GetSpeedRatio(FMath::Max(Now, Motion.HitStopEndsAt)), Ratio);
            Motion.SlowRecoveryStartedAt = FMath::Max(Motion.SlowRecoveryStartedAt, HitTime + HoldDuration);
            Motion.SlowEndsAt = FMath::Max(Motion.SlowEndsAt, Motion.SlowRecoveryStartedAt + Duration);
            if (Motion.bCrawling)
            {
                Motion.HitStopEndsAt = FMath::Max(Motion.HitStopEndsAt, HitTime + Settings->HitStopDuration);
                continue;
            }
        }
    });
}
