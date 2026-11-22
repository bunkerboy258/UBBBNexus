#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterVisualizationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassCommonTypes.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassLODSubsystem.h"
#include "Misc/App.h"
#include "RHI.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterInitializationPendingTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterCrowdPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationSmoothingFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBMonsterPresentationStateProcessor.h"
#include "BBBMonsterVisualizationLODProcessor.h"
#include "Engine/SkeletalMesh.h"

UBBBMonsterVisualizationProcessor::UBBBMonsterVisualizationProcessor()
    : CrowdQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Presentation;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterVisualizationLODProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterPresentationStateProcessor::StaticClass()->GetFName());
}

void UBBBMonsterVisualizationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    // 保留引擎表现查询后 仅筛选小怪实体
    Super::ConfigureQueries(EntityManager);

    // 让可视化查询只处理小怪实体
    EntityQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    EntityQuery.AddTagRequirement<FBBBMonsterInitializationPendingTag>(EMassFragmentPresence::None);

    CrowdQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    CrowdQuery.AddTagRequirement<FBBBMonsterInitializationPendingTag>(EMassFragmentPresence::None);
    CrowdQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FMassRepresentationLODFragment>(EMassFragmentAccess::ReadWrite);
    CrowdQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterPresentationStateFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterPresentationSmoothingFragment>(EMassFragmentAccess::ReadWrite);
    CrowdQuery.AddRequirement<FBBBMonsterGoreFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterHitReactionFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterVariationFragment>(EMassFragmentAccess::ReadOnly);
    CrowdQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
}

void UBBBMonsterVisualizationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (!World || !FApp::CanEverRender() || GUsingNullRHI || World->GetNetMode() == NM_DedicatedServer)
    {
        Super::Execute(EntityManager, Context);
        return;
    }
    auto* Crowd = World->GetSubsystem<UBBBMonsterCrowdPresentationSubsystem>();
    auto* LOD = World->GetSubsystem<UMassLODSubsystem>();
    if (!Crowd || !LOD)
    {
        Super::Execute(EntityManager, Context);
        return;
    }

    TRACE_CPUPROFILER_EVENT_SCOPE(BBBMonsterUpdateCrowdPresentation);
    TArray<FVector, TInlineAllocator<2>> Views;
    for (const auto& Viewer : LOD->GetViewers())
    {
        if (const auto* Controller = Viewer.GetPlayerController();
            Viewer.Handle.IsValid() && Viewer.bEnabled && Controller && Controller->GetLocalPlayer())
        {
            Views.Add(Viewer.Location);
        }
    }

    TMap<FMassEntityHandle, FMassRepresentationLODFragment> Restore;
    const float Now = World->GetTimeSeconds();
    const float DeltaSeconds = FMath::Max(0.0f, Context.GetDeltaTimeSeconds());
    const float Alpha = 1.0f - FMath::Exp(-DeltaSeconds / 0.055f);
    if (!Views.IsEmpty())
    {
        CrowdQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
        {
            auto Levels = Chunk.GetMutableFragmentView<FMassRepresentationLODFragment>();
            auto Smoothing = Chunk.GetMutableFragmentView<FBBBMonsterPresentationSmoothingFragment>();
            const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
            const auto Networks = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
            const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
            const auto States = Chunk.GetFragmentView<FBBBMonsterPresentationStateFragment>();
            const auto Gore = Chunk.GetFragmentView<FBBBMonsterGoreFragment>();
            const auto Traversal = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
            const auto Mobility = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
            const auto Hits = Chunk.GetFragmentView<FBBBMonsterHitReactionFragment>();
            const auto Variation = Chunk.GetFragmentView<FBBBMonsterVariationFragment>();
            const auto Specials = Chunk.GetFragmentView<FBBBMonsterSpecialFragment>();
            for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
            {
                auto& Level = Levels[Index];
                if (Level.LOD >= EMassLOD::Off || !Networks[Index].Definition.IsValid())
                {
                    Smoothing[Index].bFromCrowd = false;
                    continue;
                }
                const auto& Definition = *Networks[Index].Definition;
                const auto* Settings = Definition.Presentation.Get();
                if (!Settings || !Settings->Gore || !Settings->Gore->Body)
                {
                    continue;
                }

                const auto Entity = Chunk.GetEntity(Index);
                const bool bExistingCrowd = Crowd->HasPresentation(Entity);
                const auto& Transform = Transforms[Index].GetTransform();
                double DistanceSquared = TNumericLimits<double>::Max();
                for (const auto& View : Views)
                {
                    DistanceSquared = FMath::Min(DistanceSquared, FVector::DistSquared(Transform.GetLocation(), View));
                }
                const auto& State = States[Index];
                const bool bCorpse = State.State == EBBBMonsterBehavior::Dead;
                const bool bSettled = bCorpse && Now - State.StateEnteredTime >=
                    Settings->Animation.DeathAnimationDuration + Settings->Animation.CorpseSimulationDuration;
                bool bDamaged = false;
                for (const uint8 Preset : Gore[Index].DismembermentPresets)
                {
                    bDamaged |= Preset != 0;
                }
                for (const uint8 Strength : Gore[Index].DestructionStrengths)
                {
                    bDamaged |= Strength != 0;
                }
                const bool bCritical = State.State == EBBBMonsterBehavior::Attack || (bCorpse && !bSettled)
                    || bDamaged || Hits[Index].Age < 0.35f || Traversal[Index].Fact.Kind != 0
                    || Mobility[Index].bCrawling || Mobility[Index].IsStaggering(Now) || Mobility[Index].IsShocked(Now)
                    || (!Specials.IsEmpty() && Specials[Index].Fact.Phase != 0);
                const bool bUseCrowd = !bCritical && DistanceSquared >=
                    FMath::Square(bExistingCrowd ? 1850.0 : 2200.0);
                if (!bUseCrowd && (!bExistingCrowd || Actors[Index].Get()))
                {
                    continue;
                }

                const int32 Style = FMath::Min<int32>(Variation[Index].LocomotionStyle, 2);
                int32 Sequence = Style;
                const float Speed = State.Speed / FMath::Max(0.01f, Variation[Index].SpeedScale);
                if (State.State == EBBBMonsterBehavior::Patrol && Speed > 15.0f)
                {
                    Sequence = 3 + Style;
                }
                if (State.State == EBBBMonsterBehavior::Chase && Speed > 15.0f)
                {
                    const double RunThreshold = FMath::Lerp(Definition.WalkerChaseWalkSpeed[Style], Definition.RunSpeed[Style], 0.45);
                    Sequence = (Speed > RunThreshold ? 9 : 6) + Style;
                }
                if (bSettled)
                {
                    Sequence = 12;
                }

                const FTransform MeshTransform = Settings->MeshTransform * Transform;
                FTransform Display;
                if (!Crowd->Present(Entity, *Settings->Gore->Body, MeshTransform, Sequence, State.LoopPhase,
                    bSettled, Alpha, DeltaSeconds, Display))
                {
                    continue;
                }
                Smoothing[Index].DisplayTransform = Settings->MeshTransform.Inverse() * Display;
                Smoothing[Index].bFromCrowd = true;
                if (bUseCrowd)
                {
                    Restore.Add(Entity, Level);
                    Level.LOD = EMassLOD::Off;
                    Level.LODSignificance = float(EMassLOD::Off);
                }
            }
        });
    }
    Super::Execute(EntityManager, Context);
    if (!Restore.IsEmpty())
    {
        CrowdQuery.ForEachEntityChunk(Context, [&Restore](FMassExecutionContext& Chunk)
        {
            auto Levels = Chunk.GetMutableFragmentView<FMassRepresentationLODFragment>();
            for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
            {
                if (const auto* Original = Restore.Find(Chunk.GetEntity(Index)))
                {
                    Levels[Index] = *Original;
                }
            }
        });
    }
    Crowd->EndFrame();
}
