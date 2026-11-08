#include "BBBMonsterSpitProcessor.h"

#include "MassCommonFragments.h"
#include "Engine/World.h"
#include "MassMovementFragments.h"
#include "MassExecutionContext.h"
#include "MassCommands.h"
#include "MassActorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpecialDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Behavior/BBBMonsterBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Processors/BBBSlimeProcessor.h"

UBBBMonsterSpitProcessor::UBBBMonsterSpitProcessor() : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterBehaviorProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteBefore.Add(BBBMassProcessingGroups::Movement);
}

void UBBBMonsterSpitProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterCombatFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
}

void UBBBMonsterSpitProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (!World)
    {
        return;
    }
    const float Now = World->GetTimeSeconds();
    MonsterQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        auto Specials = Chunk.GetMutableFragmentView<FBBBMonsterSpecialFragment>();
        auto Combats = Chunk.GetMutableFragmentView<FBBBMonsterCombatFragment>();
        auto Positions = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Speeds = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Injury = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Ground = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Traversal = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 I = 0; I < Specials.Num(); ++I)
        {
            const auto* Definition = Rules[I].Definition.Get();
            const auto* Rule = Definition ? Definition->Special.Get() : nullptr;
            if (!Rule || Rule->Kind != EBBBMonsterSpecialKind::Spitter || !Rule->Slime)
            {
                continue;
            }
            auto& State = Specials[I];
            auto& Fact = State.Fact;
            if (Health[I].CurrentHealth <= 0 || Injury[I].bCrawling || Injury[I].IsStaggering(Now)
                || Traversal[I].Fact.Kind != 0 || !Ground[I].bGrounded)
            {
                Fact.Phase = 0;
                State.ReadyAt = FMath::Max(State.ReadyAt, Now + Rule->Cooldown);
                continue;
            }
            if (Fact.Phase == 3)
            {
                Speeds[I].Value = FVector::ZeroVector;
                if (Now >= Fact.StartedAt + Fact.Duration)
                {
                    Fact.Phase = 0;
                }
                continue;
            }
            const FVector Location = Positions[I].GetTransform().GetLocation();
            if (Fact.Phase == 0)
            {
                const float Distance = FVector::Dist2D(Location, Targets[I].TargetLocation);
                if (Now < State.ReadyAt || !Targets[I].bTargetVisible || !Targets[I].TargetActor.IsValid()
                    || Distance < Rule->MinimumRange || Distance > Rule->MaximumRange)
                {
                    continue;
                }
                ++Fact.Serial;
                Fact.Phase = 4;
                Fact.StartedAt = Now;
                Fact.Duration = Rule->Windup;
                Fact.Goal = Targets[I].TargetLocation - FVector::UpVector * 60;
                Combats[I].AttackTarget.Reset();
                Combats[I].bHitAttempted = true;
                Combats[I].bAttackFinished = true;
                Positions[I].GetMutableTransform().SetRotation((Fact.Goal - Location).GetSafeNormal2D().ToOrientationQuat());
            }
            Speeds[I].Value = FVector::ZeroVector;
            if (Fact.Phase != 4 || Now < Fact.StartedAt + Fact.Duration)
            {
                continue;
            }
            FBBBSlimeSpawnLocalControlPacket Packet;
            Packet.Fact.InstanceId = FGuid::NewGuid();
            Packet.Fact.SourceId = Rules[I].InstanceId;
            Packet.Fact.Revision = 1;
            Packet.Fact.Definition = Rule->Slime;
            Packet.Fact.Location = Positions[I].GetTransform().TransformPosition(Rule->SpitOriginOffset);
            const FVector Offset = Fact.Goal - Packet.Fact.Location;
            const float Time = FMath::Clamp(Offset.Size2D() / Rule->Slime->Speed, 0.1f, Rule->Slime->FlightLifetime);
            Packet.Fact.Velocity = Offset / Time - FVector::UpVector * (World->GetGravityZ() * Rule->Slime->GravityScale * Time * 0.5f);
            Packet.Fact.EndsAt = Now + Rule->Slime->FlightLifetime;
            Packet.SourceActor = !Actors.IsEmpty() ? const_cast<AActor*>(Actors[I].Get()) : nullptr;
            const TWeakObjectPtr<UWorld> WeakWorld = World;
            Chunk.Defer().PushCommand<FMassDeferredCreateCommand>([WeakWorld, Packet](FMassEntityManager&)
            {
                auto* CurrentWorld = WeakWorld.Get();
                auto* Config = Packet.Fact.Definition.Get();
                if (!CurrentWorld || !Config || !Config->IsValid())
                {
                    return;
                }
                int32 SourceCount = 0;
                const int32 Total = UBBBSlimeProcessor::Count(*CurrentWorld, Packet.Fact.SourceId, SourceCount);
                if (Total >= Config->WorldLimit || SourceCount >= Config->SourceLimit)
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[BBBSlime] 已达到存活额度 World=%d Source=%d"), Total, SourceCount);
                    return;
                }
                auto* Mass = CurrentWorld->GetSubsystem<UBBBMassSubsystem>();
                const auto Entity = Mass->CreateEntity(*Config->EntityConfig);
                if (Entity.IsSet())
                {
                    Mass->SubmitInput(Entity, Packet);
                }
            });
            Fact.Phase = 3;
            Fact.StartedAt = Now;
            Fact.Duration = Rule->Recovery;
            State.ReadyAt = Now + Rule->Recovery + Rule->Cooldown;
        }
    });
}
