#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationSmoothingFragment.h"
#include "Engine/World.h"
#include "MassActorSubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationStateProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterVisualizationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterSoundPresentationComponent.h"

UBBBMonsterPresentationProcessor::UBBBMonsterPresentationProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Presentation;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterPresentationStateProcessor::StaticClass()->GetFName());
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterVisualizationProcessor::StaticClass()->GetFName());
}

void UBBBMonsterPresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterHitReactionFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPresentationStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPresentationSmoothingFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterPresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    const UWorld* World = Context.GetWorld();
    const float DeltaTime = Context.GetDeltaTimeSeconds();

    if (!ensureMsgf(World != nullptr && FMath::IsFinite(DeltaTime) && DeltaTime >= 0.0f,
        TEXT("[UBBBM]Monster presentation smoothing requires a valid world and time step")))
    {
        return;
    }

    const bool bRemote = World->GetNetMode() == NM_Client;
    const float Alpha = 1.0f - FMath::Exp(-DeltaTime / 0.1f);

    const float Now = World->GetTimeSeconds();
    const bool bStandalone = World->GetNetMode() == NM_Standalone;
    const uint32 WorldIdentity = World->GetUniqueID();
    MonsterQuery.ForEachEntityChunk(Context, [bRemote, Alpha, Now, bStandalone, WorldIdentity](FMassExecutionContext& ChunkContext)
    {
        // 表现层只读取逻辑结果 不参与决策
        TArrayView<FMassActorFragment> Actors = ChunkContext.GetMutableFragmentView<FMassActorFragment>();
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FBBBMonsterAvoidanceFragment> Avoidances = ChunkContext.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const TConstArrayView<FBBBMonsterPresentationStateFragment> PresentationStates = ChunkContext.GetFragmentView<FBBBMonsterPresentationStateFragment>();
        auto SmoothingStates = ChunkContext.GetMutableFragmentView<FBBBMonsterPresentationSmoothingFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            // 取得实体对应的表现演员
            ABBBMonsterPresentationActor* MonsterActor = Cast<ABBBMonsterPresentationActor>(Actors[Index].GetMutable());
            FBBBMonsterPresentationSmoothingFragment& Smoothing = SmoothingStates[Index];

            if (!IsValid(MonsterActor))
            {
                Smoothing.LastActor.Reset();
                continue;
            }

            const FTransform& MonsterTransform = Transforms[Index].GetTransform();

            if (!ensureMsgf(!MonsterTransform.ContainsNaN(), TEXT("[UBBBM]Monster presentation received an invalid transform")))
            {
                Smoothing.LastActor.Reset();
                continue;
            }

            const bool bNewActor = Smoothing.LastActor.Get() != MonsterActor;
            if (bNewActor)
            {
                MonsterActor->SetActorEnableCollision(true);
                if (auto* Reaction = MonsterActor->FindComponentByClass<UBBBMonsterHitReactionComponent>())
                {
                    Reaction->ResetPresentation();
                }
            }
            const double DistanceSquared = FVector::DistSquared(Smoothing.DisplayTransform.GetLocation(), MonsterTransform.GetLocation());
            const bool bLargeCorrection = DistanceSquared > FMath::Square(500.0);

            if (!bRemote || bNewActor || bLargeCorrection)
            {
                if (bRemote && !bNewActor && bLargeCorrection)
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Client presentation snap Entity=%d Distance=%.1f"),
                        ChunkContext.GetEntity(Index).Index, FMath::Sqrt(DistanceSquared));
                }

                Smoothing.DisplayTransform = MonsterTransform;
            }

            // 只追靠最新已成立结果 不使用速度外推 不回写逻辑位置
            if (bRemote && !bNewActor && !bLargeCorrection)
            {
                Smoothing.DisplayTransform.SetLocation(FMath::Lerp(
                    Smoothing.DisplayTransform.GetLocation(), MonsterTransform.GetLocation(), Alpha));
                Smoothing.DisplayTransform.SetRotation(FQuat::Slerp(
                    Smoothing.DisplayTransform.GetRotation(), MonsterTransform.GetRotation(), Alpha).GetNormalized());

                if (Smoothing.DisplayTransform.GetLocation().Equals(MonsterTransform.GetLocation(), 0.01))
                {
                    Smoothing.DisplayTransform.SetLocation(MonsterTransform.GetLocation());
                }

                if (Smoothing.DisplayTransform.GetRotation().Equals(MonsterTransform.GetRotation(), 0.0001))
                {
                    Smoothing.DisplayTransform.SetRotation(MonsterTransform.GetRotation());
                }
            }

            Smoothing.LastActor = MonsterActor;

            // 同步实体位置和朝向到骨骼表现 Actor
            MonsterActor->SetActorLocationAndRotation(
                Smoothing.DisplayTransform.GetLocation(),
                Smoothing.DisplayTransform.GetRotation(),
                false,
                nullptr,
                ETeleportType::TeleportPhysics);

            // 取得表现组件同步状态和移动速度
            UBBBMonsterPresentationComponent* Presentation = MonsterActor->GetMonsterPresentation();

            if (!ensureMsgf(Presentation != nullptr, TEXT("[UBBBM]Monster actor requires BBBMonsterPresentationComponent")))
            {
                continue;
            }

            const FBBBMonsterPresentationStateFragment& PresentationState = PresentationStates[Index];
            const auto& Mobility = ChunkContext.GetFragmentView<FBBBMonsterMobilityFragment>()[Index];
            const auto* Definition = ChunkContext.GetFragmentView<FBBBMonsterNetworkFragment>()[Index].Definition.Get();
            if (Definition)
            {
                const float Progress = Mobility.bCrawling ? FMath::Clamp((Now - Mobility.CrawlStartedAt) / Definition->CrawlTransitionDuration, 0.0f, 1.0f) : 0.0f;
                Presentation->ApplyMobilityState(Mobility.bCrawling, Progress,
                    FMath::Lerp(Definition->CapsuleHalfHeight, Definition->CrawlCapsuleHalfHeight, Progress));
            }
            // 将状态和速度交给表现组件选择动画
            Presentation->ApplyPresentationState(
                PresentationState.State,
                Velocities[Index].Value.Size2D(),
                PresentationState.StateEnteredTime,
                PresentationState.ActionId,
                PresentationState.ActionProgress);
            Presentation->ApplyHitReaction(ChunkContext.GetFragmentView<FBBBMonsterHitReactionFragment>()[Index]);
            if (auto* SoundPresentation = MonsterActor->GetMonsterSoundPresentation())
            {
                // 单机没有网络身份 声音使用世界内完整代际句柄 不创建网络事实
                const FMassEntityHandle Entity = ChunkContext.GetEntity(Index);
                const FGuid SoundInstance = bStandalone
                    ? FGuid(WorldIdentity, static_cast<uint32>(Entity.Index), static_cast<uint32>(Entity.SerialNumber), 1u)
                    : ChunkContext.GetFragmentView<FBBBMonsterNetworkFragment>()[Index].InstanceId;
                if (!SoundInstance.IsValid())
                {
                    continue;
                }

                SoundPresentation->ApplyFacts(
                    SoundInstance,
                    Definition ? Definition->SoundPresentation.Get() : nullptr,
                    PresentationState.State, PresentationState.ActionId, PresentationState.ActionProgress,
                    ChunkContext.GetFragmentView<FBBBMonsterHitReactionFragment>()[Index],
                    Mobility.bCrawling, Velocities[Index].Value.Size2D(), Now, bNewActor);
            }
        }
    });
}
