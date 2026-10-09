#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Perception/BBBMonsterPerceptionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterPerceptionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterStimulusFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"

UBBBMonsterPerceptionProcessor::UBBBMonsterPerceptionProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    bRequiresGameThreadExecution = true;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterDamageProcessor::StaticClass()->GetFName());
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
}

void UBBBMonsterPerceptionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPerceptionFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterDamageFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterStimulusFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterPerceptionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster perception requires a valid world")))
    {
        return;
    }
    if (World->GetNetMode() == NM_Client)
    {
        return;
    }
    const float Now = World->GetTimeSeconds();
    const float FrameDelta = Context.GetDeltaTimeSeconds();
    const auto* GameState = World->GetGameState();
    const double ServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : Now;

    // 获取当前世界中的玩家目标
    TArray<APawn*, TInlineAllocator<8>> Players;
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        if (It->IsValid() && IsValid(It->Get()->GetPawn()) && It->Get()->GetPawn()->CanBeDamaged())
        {
            Players.Add(It->Get()->GetPawn());
        }
    }
    TArray<TPair<FMassEntityHandle, FVector>> Alerts;
    TArray<FVector> AlertOrigins;
    TArray<float> AlertRanges;
    int32 TraceBudget = 512;
    const int32 EntityCount = MonsterQuery.GetNumMatchingEntities();
    if (EntityCount == 0)
    {
        return;
    }
    const int32 FirstSenseIndex = NextSenseEntityIndex % EntityCount;
    int32 EntityIndex = 0;
    bool bWrap = false;

    // 每帧根据视野范围更新目标请求
    const auto SenseChunk = [this, &Players, &Alerts, &AlertOrigins, &AlertRanges, &TraceBudget, &EntityIndex, &bWrap,
        FirstSenseIndex, EntityCount, World, Now, ServerTime, FrameDelta](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        auto Perceptions = Chunk.GetMutableFragmentView<FBBBMonsterPerceptionFragment>();
        auto Targets = Chunk.GetMutableFragmentView<FBBBMonsterTargetFragment>();
        const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Damage = Chunk.GetFragmentView<FBBBMonsterDamageFragment>();
        const auto Navigation = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Stimuli = Chunk.GetFragmentView<FBBBMonsterStimulusFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            const int32 CurrentIndex = EntityIndex++;
            if ((!bWrap && CurrentIndex < FirstSenseIndex) || (bWrap && CurrentIndex >= FirstSenseIndex))
            {
                continue;
            }
            auto& Sense = Perceptions[Index];
            auto& Target = Targets[Index];
            const auto* Settings = Networks[Index].Definition.Get();
            if (!ensureMsgf(Settings, TEXT("[BBBPerception]Missing monster definition")))
            {
                continue;
            }
            const auto ClearTarget = [&Target]()
            {
                if (Target.bHasTarget)
                {
                    ++Target.Revision;
                }
                Target.bHasTarget = false;
                Target.bTargetVisible = false;
                Target.TargetActor.Reset();
            };
            if (Target.TargetActor.IsStale() || (Target.TargetActor.IsValid() && !Target.TargetActor->CanBeDamaged()))
            {
                ClearTarget();
            }
            if (Health[Index].CurrentHealth <= 0.0f)
            {
                ClearTarget();
                Sense.Contacts.Reset();
                continue;
            }
            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const auto& Path = Navigation[Index];
            if (Target.bHasTarget && !Target.bTargetVisible && Path.TargetRevision == Target.Revision && Path.bReachedDestination)
            {
                Sense.InvestigationFinishedAt = Now;
                for (auto& Contact : Sense.Contacts)
                {
                    Contact.bConfirmed = false;
                    Contact.Awareness = 0.0f;
                }
                ClearTarget();
            }
            if (Now < Sense.NextSenseTime || TraceBudget < Players.Num() * 3)
            {
                continue;
            }
            const float Delta = Sense.LastSenseTime < 0.0f ? FrameDelta : FMath::Min(Now - Sense.LastSenseTime, 0.25f);
            Sense.LastSenseTime = Now;
            Sense.NextSenseTime = Now + 0.1f;
            NextSenseEntityIndex = (CurrentIndex + 1) % EntityCount;
            Sense.Contacts.RemoveAll([&Players](const auto& Contact)
            {
                return !Contact.Actor.IsValid() || !Players.Contains(Cast<APawn>(Contact.Actor.Get()));
            });
            const FVector Eye = Location + FVector::UpVector * (Settings->CapsuleHalfHeight * 0.65f);
            const FVector Facing = Transforms[Index].GetTransform().GetRotation().GetForwardVector().GetSafeNormal2D();
            for (APawn* Player : Players)
            {
                auto* Found = Sense.Contacts.FindByPredicate([Player](const auto& Contact)
                {
                    return Contact.Actor == Player;
                });
                if (!Found)
                {
                    FBBBMonsterVisualContact Contact;
                    Contact.Actor = Player;
                    Sense.Contacts.Add(Contact);
                    Found = &Sense.Contacts.Last();
                }
                auto& Contact = *Found;
                Contact.bVisible = false;
                const FVector PlayerLocation = Player->GetActorLocation();
                const float Distance = FVector::Distance(Location, PlayerLocation);
                const FVector Direction = (PlayerLocation - Location).GetSafeNormal2D();
                int32 VisibleSamples = 0;
                if (Distance <= Sense.SightRange && FVector::DotProduct(Facing, Direction) >=
                    FMath::Cos(FMath::DegreesToRadians(Settings->SightAngle * 0.5f)))
                {
                    FVector Center = PlayerLocation;
                    float HalfHeight = 80.0f;
                    if (const auto* Body = Cast<UPrimitiveComponent>(Player->GetRootComponent()))
                    {
                        Center = Body->Bounds.Origin;
                        HalfHeight = FMath::Clamp(static_cast<float>(Body->Bounds.BoxExtent.Z), 20.0f, 100.0f);
                    }
                    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterVision), false);
                    Params.AddIgnoredActor(Player);
                    if (!Actors.IsEmpty() && Actors[Index].Get())
                    {
                        Params.AddIgnoredActor(Actors[Index].Get());
                    }
                    for (float Height : {0.65f, 0.0f, -0.5f})
                    {
                        --TraceBudget;
                        FHitResult Blocker;
                        if (!World->LineTraceSingleByChannel(Blocker, Eye, Center + FVector::UpVector * HalfHeight * Height, ECC_Visibility, Params))
                        {
                            ++VisibleSamples;
                        }
                    }
                }
                Contact.bVisible = VisibleSamples > 0;
                if (Contact.bVisible)
                {
                    const float Exposure = VisibleSamples / 3.0f;
                    const float Motion = FMath::Clamp(static_cast<float>(Player->GetVelocity().Size2D()) / 600.0f, 0.0f, 1.0f);
                    const float Difficulty = FMath::Clamp(Distance / FMath::Max(Sense.SightRange, 1.0f) + (1.0f - Exposure) * 0.35f - Motion * 0.2f, 0.0f, 1.0f);
                    const float ConfirmDuration = FMath::Lerp(Settings->SightConfirmMin, Settings->SightConfirmMax, Difficulty);
                    Contact.Awareness = FMath::Min(1.0f, Contact.Awareness + FMath::Max(Delta, 0.0f) / ConfirmDuration);
                    Contact.bConfirmed = Contact.bConfirmed || Contact.Awareness >= 1.0f;
                    if (Contact.bConfirmed)
                    {
                        Contact.Position = PlayerLocation;
                        Contact.ConfirmedAt = Now;
                    }
                }
                if (!Contact.bVisible)
                {
                    Contact.Awareness = FMath::Max(0.0f, Contact.Awareness - FMath::Max(Delta, 0.0f) / Settings->AwarenessDecayDuration);
                    if (Now - Contact.ConfirmedAt >= Settings->TargetMemoryDuration)
                    {
                        Contact.bConfirmed = false;
                    }
                }
                if (Path.ExcludedUntil > Now && FVector::DistSquared(Path.ExcludedPosition, Contact.Position) < FMath::Square(200.0f))
                {
                    Contact.ExcludedUntil = Path.ExcludedUntil;
                    Contact.ExcludedPosition = Path.ExcludedPosition;
                }
            }
            FBBBMonsterVisualContact* Best = nullptr;
            FBBBMonsterVisualContact* Current = nullptr;
            float BestDistance = FLT_MAX;
            for (auto& Contact : Sense.Contacts)
            {
                if (!Contact.bConfirmed || !Contact.Actor.IsValid())
                {
                    continue;
                }
                if (Now < Contact.ExcludedUntil && FVector::DistSquared(Contact.Position, Contact.ExcludedPosition) < FMath::Square(200.0f))
                {
                    continue;
                }
                const float Distance = FVector::Distance(Location, Contact.Position);
                if (Distance > Settings->TargetLeashDistance)
                {
                    continue;
                }
                if (Contact.Actor == Target.TargetActor)
                {
                    Current = &Contact;
                }
                if (Contact.bVisible && Distance < BestDistance)
                {
                    Best = &Contact;
                    BestDistance = Distance;
                }
            }
            auto* Selected = Current;
            if (!Current)
            {
                Selected = Best;
                Sense.SwitchCandidate.Reset();
            }
            if (Current && Best && Best != Current)
            {
                const float CurrentDistance = FVector::Distance(Location, Current->Position);
                const bool bBetter = BestDistance <= CurrentDistance * Settings->TargetSwitchRatio &&
                    CurrentDistance - BestDistance >= Settings->TargetSwitchAdvantage;
                if (bBetter)
                {
                    if (Sense.SwitchCandidate != Best->Actor)
                    {
                        Sense.SwitchCandidate = Best->Actor;
                        Sense.SwitchStartedAt = Now;
                    }
                    if (Now - Sense.SwitchStartedAt >= Settings->TargetSwitchDuration)
                    {
                        Selected = Best;
                        Sense.SwitchCandidate.Reset();
                    }
                }
                if (!bBetter)
                {
                    Sense.SwitchCandidate.Reset();
                }
            }
            if (!Best || Best == Current)
            {
                Sense.SwitchCandidate.Reset();
            }

            const FBBBMonsterDamageContribution* Latest = nullptr;
            for (const auto& Pair : Damage[Index].Contributions)
            {
                if (Pair.Value.Parts.Sum() > 0.0 && (!Latest || Pair.Value.LastHitTime > Latest->LastHitTime))
                {
                    Latest = &Pair.Value;
                }
            }
            bool bDamageEvidence = false;
            FVector EvidencePosition = FVector::ZeroVector;
            float EvidenceTime = -FLT_MAX;
            if (Latest && Latest->LastHitTime > Sense.LastDamageTime)
            {
                Sense.LastDamageTime = Latest->LastHitTime;
                const double Age = ServerTime - Latest->LastHitTime;
                if (Latest->bHasSourcePosition && Age >= -0.1 && Age < Settings->TargetMemoryDuration)
                {
                    bDamageEvidence = true;
                    EvidencePosition = Latest->LastSourcePosition;
                    EvidenceTime = Now - static_cast<float>(FMath::Max(Age, 0.0));
                }
            }
            if (!Selected && Stimuli[Index].Time <= Now && Now < Stimuli[Index].EndsAt &&
                Stimuli[Index].Time > Sense.InvestigationFinishedAt && Stimuli[Index].Time > Target.EvidenceTime &&
                Stimuli[Index].Time > EvidenceTime)
            {
                EvidencePosition = Stimuli[Index].Position;
                EvidenceTime = Stimuli[Index].Time;
            }
            if (Selected && (!bDamageEvidence || Selected->bVisible))
            {
                if (Target.TargetActor != Selected->Actor || !Target.bHasTarget)
                {
                    ++Target.Revision;
                }
                // 记录玩家当前位置供导航处理器使用
                Target.TargetLocation = Selected->Position;
                Target.LastSeenTime = Selected->ConfirmedAt;
                Target.EvidenceTime = Selected->ConfirmedAt;
                Target.bHasTarget = true;
                Target.bTargetVisible = Selected->bVisible;
                Target.TargetActor = Selected->Actor;
                if (Selected->bVisible && Now >= Sense.NextAllyAlertTime)
                {
                    Alerts.Emplace(Chunk.GetEntity(Index), Selected->Position);
                    AlertOrigins.Add(Location);
                    AlertRanges.Add(Settings->AllyAlertRange);
                    Sense.NextAllyAlertTime = Now + Settings->AllyAlertCooldown;
                }
                continue;
            }
            if (EvidenceTime > Sense.InvestigationFinishedAt && EvidenceTime > Target.EvidenceTime &&
                FVector::DistSquared(Location, EvidencePosition) <= FMath::Square(Settings->TargetLeashDistance))
            {
                ++Target.Revision;
                Target.TargetActor.Reset();
                Target.TargetLocation = EvidencePosition;
                Target.EvidenceTime = EvidenceTime;
                Target.LastSeenTime = EvidenceTime;
                Target.bHasTarget = true;
                Target.bTargetVisible = false;
                continue;
            }
            if (Target.bHasTarget && !Target.TargetActor.IsValid() && Now - Target.EvidenceTime < Settings->TargetMemoryDuration &&
                FVector::DistSquared(Location, Target.TargetLocation) <= FMath::Square(Settings->TargetLeashDistance) &&
                !(Path.ExcludedUntil > Now && FVector::DistSquared(Path.ExcludedPosition, Target.TargetLocation) < FMath::Square(200.0f)))
            {
                Target.bTargetVisible = false;
                continue;
            }
            // 玩家无效或超出范围时清除目标并进入侦察状态
            ClearTarget();
        }
    };
    MonsterQuery.ForEachEntityChunk(Context, SenseChunk);
    if (FirstSenseIndex > 0)
    {
        bWrap = true;
        EntityIndex = 0;
        MonsterQuery.ForEachEntityChunk(Context, SenseChunk);
    }

    // 同伴仅接收本轮自己视觉确认产生的位置 不转发收到的线索
    MonsterQuery.ForEachEntityChunk(Context, [&Alerts, &AlertOrigins, &AlertRanges, Now](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        auto Targets = Chunk.GetMutableFragmentView<FBBBMonsterTargetFragment>();
        const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Navigation = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Perceptions = Chunk.GetFragmentView<FBBBMonsterPerceptionFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Target = Targets[Index];
            if (Health[Index].CurrentHealth <= 0.0f || Target.bHasTarget || !Networks[Index].Definition.IsValid() ||
                Now < Perceptions[Index].InvestigationFinishedAt + Networks[Index].Definition->InvestigationAlertDuration)
            {
                continue;
            }
            for (int32 AlertIndex = 0; AlertIndex < Alerts.Num(); ++AlertIndex)
            {
                if (Alerts[AlertIndex].Key == Chunk.GetEntity(Index) || FVector::DistSquared(
                    Transforms[Index].GetTransform().GetLocation(), AlertOrigins[AlertIndex]) > FMath::Square(AlertRanges[AlertIndex]))
                {
                    continue;
                }
                if (Navigation[Index].ExcludedUntil > Now && FVector::DistSquared(
                    Navigation[Index].ExcludedPosition, Alerts[AlertIndex].Value) < FMath::Square(200.0f))
                {
                    continue;
                }
                Target.TargetLocation = Alerts[AlertIndex].Value;
                Target.LastSeenTime = Now;
                Target.EvidenceTime = Now;
                Target.bHasTarget = true;
                Target.bTargetVisible = false;
                Target.TargetActor.Reset();
                ++Target.Revision;
                break;
            }
        }
    });
}
