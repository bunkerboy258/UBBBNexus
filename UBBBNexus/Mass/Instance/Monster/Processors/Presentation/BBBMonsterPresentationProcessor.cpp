#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationSmoothingFragment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Math/RandomStream.h"
#include "Misc/App.h"
#include "MassActorSubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassLODFragments.h"
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
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterInitializationPendingTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDeathFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGorePresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterGoreProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGibPresentationSubsystem.h"
#include "ProfilingDebugging/CountersTrace.h"

TRACE_DECLARE_INT_COUNTER(BBBMonsterBodyActors, TEXT("BBBMonster/BodyActors"));
TRACE_DECLARE_INT_COUNTER(BBBMonsterSimulatingCorpses, TEXT("BBBMonster/SimulatingCorpses"));

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
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterGoreProcessor::StaticClass()->GetFName());
}

void UBBBMonsterPresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassViewerInfoFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddRequirement<FBBBMonsterHitReactionFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterVariationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterDeathFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGoreFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPresentationStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPresentationSmoothingFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    MonsterQuery.AddTagRequirement<FBBBMonsterInitializationPendingTag>(EMassFragmentPresence::None);
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
    World->GetSubsystem<UBBBMonsterGibPresentationSubsystem>()->AdvancePresentation(Now);
    const bool bStandalone = World->GetNetMode() == NM_Standalone;
    const uint32 WorldIdentity = World->GetUniqueID();
    int32 ActiveCorpses = 0;
    int32 ActiveBurningVfx = 0;
    int32 BodyActors = 0;
    MonsterQuery.ForEachEntityChunk(Context, [Now, &ActiveCorpses, &ActiveBurningVfx, &BodyActors](FMassExecutionContext& Chunk)
    {
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto* Actor = Cast<ABBBMonsterPresentationActor>(Actors[Index].Get());
            BodyActors += Actor != nullptr ? 1 : 0;
            ActiveBurningVfx += (Actor && Actor->HasBurningPresentation()) ? 1 : 0;
            auto* Presentation = Actor ? Actor->GetMonsterPresentation() : nullptr;
            const auto* Definition = Network[Index].Definition.Get();
            if (Presentation && Definition && Definition->Presentation && Presentation->IsCorpseSimulating())
            {
                Presentation->UpdateCorpsePresentation(Now, Definition->Presentation->Animation.CorpseSimulationDuration);
                ActiveCorpses += Presentation->IsCorpseSimulating() ? 1 : 0;
            }
        }
    });
    TRACE_COUNTER_SET(BBBMonsterBodyActors, BodyActors);
    TRACE_COUNTER_SET(BBBMonsterSimulatingCorpses, ActiveCorpses);
    int32 PreparationBudget = World->GetNetMode() != NM_DedicatedServer && FApp::CanEverRender() ? 2 : 0;
    int32 CorpseActivationsRemaining = 4;
    constexpr int32 MaxActiveBurningVfx = 14;
    MonsterQuery.ForEachEntityChunk(Context, [&PreparationBudget](FMassExecutionContext &Chunk)
    {
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        const auto Rules = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Facts = Chunk.GetFragmentView<FBBBMonsterGoreFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities() && PreparationBudget > 0; ++Index)
        {
            auto *Actor = Cast<ABBBMonsterPresentationActor>(Actors[Index].Get());
            const auto *Rule = Rules[Index].Definition.Get();
            if (Actor && Rule && Rule->Presentation && Rule->Presentation->Gore
                && Actor->GetMonsterMesh()->WasRecentlyRendered(0.2f)
                && Actor->GetMonsterMesh()->GetPredictedLODLevel() <= 2
                && Actor->GetMonsterGore()->PrepareNext(*Rule->Presentation->Gore, Facts[Index], true))
            {
                --PreparationBudget;
            }
        }
    });
    MonsterQuery.ForEachEntityChunk(Context, [bRemote, Alpha, Now, bStandalone, WorldIdentity, &ActiveCorpses, &ActiveBurningVfx, &PreparationBudget, &CorpseActivationsRemaining](FMassExecutionContext& ChunkContext)
    {
        // 表现层只读取逻辑结果 不参与决策
        TArrayView<FMassActorFragment> Actors = ChunkContext.GetMutableFragmentView<FMassActorFragment>();
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FBBBMonsterAvoidanceFragment> Avoidances = ChunkContext.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const TConstArrayView<FBBBMonsterPresentationStateFragment> PresentationStates = ChunkContext.GetFragmentView<FBBBMonsterPresentationStateFragment>();
        auto SmoothingStates = ChunkContext.GetMutableFragmentView<FBBBMonsterPresentationSmoothingFragment>();
        const auto Specials = ChunkContext.GetFragmentView<FBBBMonsterSpecialFragment>();
        const auto MobilityStates = ChunkContext.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto NetworkStates = ChunkContext.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto GoreStates = ChunkContext.GetFragmentView<FBBBMonsterGoreFragment>();
        const auto HitReactions = ChunkContext.GetFragmentView<FBBBMonsterHitReactionFragment>();
        const auto DeathStates = ChunkContext.GetFragmentView<FBBBMonsterDeathFragment>();
        const auto Variations = ChunkContext.GetFragmentView<FBBBMonsterVariationFragment>();
        const auto Traversals = ChunkContext.GetFragmentView<FBBBMonsterTraversalFragment>();
        const auto Grounds = ChunkContext.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Health = ChunkContext.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Viewers = ChunkContext.GetFragmentView<FMassViewerInfoFragment>();

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
                MonsterActor->GetMonsterPresentation()->ResetCorpsePresentation();
                MonsterActor->GetMonsterGore()->ResetPresentation();
                MonsterActor->SetActorEnableCollision(true);
                if (auto* Reaction = MonsterActor->FindComponentByClass<UBBBMonsterHitReactionComponent>())
                {
                    Reaction->ResetPresentation();
                }
            }
            const double DistanceSquared = FVector::DistSquared(Smoothing.DisplayTransform.GetLocation(), MonsterTransform.GetLocation());
            const bool bLargeCorrection = DistanceSquared > FMath::Square(500.0);

            const bool bTransferCrowd = Smoothing.bFromCrowd && !bLargeCorrection;
            if ((!bRemote || bNewActor || bLargeCorrection) && !bTransferCrowd)
            {
                if (bRemote && !bNewActor && bLargeCorrection)
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Client presentation snap Entity=%d Distance=%.1f"),
                        ChunkContext.GetEntity(Index).Index, FMath::Sqrt(DistanceSquared));
                }

                Smoothing.DisplayTransform = MonsterTransform;
            }

            // 只追靠最新已成立结果 不使用速度外推 不回写逻辑位置
            if ((bRemote && !bNewActor && !bLargeCorrection) || bTransferCrowd)
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
            Smoothing.bFromCrowd = false;

            // 同步实体位置和朝向到骨骼表现 Actor
            const FTransform& CurrentTransform = MonsterActor->GetActorTransform();
            const bool bTransformChanged = !CurrentTransform.GetLocation().Equals(Smoothing.DisplayTransform.GetLocation(), UE_KINDA_SMALL_NUMBER)
                || !CurrentTransform.GetRotation().Equals(Smoothing.DisplayTransform.GetRotation(), UE_KINDA_SMALL_NUMBER);
            if (!MonsterActor->GetMonsterPresentation()->IsCorpseActive() && bTransformChanged)
            {
                MonsterActor->SetActorLocationAndRotation(
                    Smoothing.DisplayTransform.GetLocation(),
                    Smoothing.DisplayTransform.GetRotation(),
                    false,
                    nullptr,
                    ETeleportType::TeleportPhysics);
            }

            // 取得表现组件同步状态和移动速度
            UBBBMonsterPresentationComponent* Presentation = MonsterActor->GetMonsterPresentation();
            if (!ensureMsgf(Presentation != nullptr, TEXT("[UBBBM]Monster actor requires BBBMonsterPresentationComponent")))
            {
                continue;
            }

            const FBBBMonsterSpecialFact CurrentSpecialFact = !Specials.IsEmpty() ? Specials[Index].Fact : FBBBMonsterSpecialFact{};
            float RushDisplayDistanceCm = 0;
            if (CurrentSpecialFact.Phase == 2)
            {
                // 远端冲撞的步相与已平滑的表现位置同步，始终限制在已确认的路程内。
                const FVector Forward = MonsterTransform.GetRotation().GetForwardVector().GetSafeNormal2D();
                const FVector DisplayOffset = Smoothing.DisplayTransform.GetLocation() - MonsterTransform.GetLocation();
                RushDisplayDistanceCm = FMath::Clamp(CurrentSpecialFact.RushDistanceCm + static_cast<float>(DisplayOffset | Forward),
                    0.0f, CurrentSpecialFact.RushDistanceCm);
            }
            Presentation->ApplySpecialFact(CurrentSpecialFact, Velocities[Index].Value, RushDisplayDistanceCm);

            const FBBBMonsterPresentationStateFragment& PresentationState = PresentationStates[Index];
            const auto& Mobility = MobilityStates[Index];
            const auto* Definition = NetworkStates[Index].Definition.Get();
            if (!Definition || !MonsterActor->ConfigurePresentation(Definition->Presentation))
            {
                MonsterActor->SetBurningPresentation(false);
                continue;
            }
            auto* BodyMesh = MonsterActor->GetMonsterMesh();
            const auto Behavior = PresentationState.State;
            const bool bOrdinaryMovement = Behavior == EBBBMonsterBehavior::Idle
                || Behavior == EBBBMonsterBehavior::Patrol || Behavior == EBBBMonsterBehavior::Chase;
            if (Behavior != EBBBMonsterBehavior::Dead)
            {
                const bool bScreenPose = !Viewers.IsEmpty() && Viewers[Index].ClosestViewerDistanceSq > FMath::Square(3500.0f)
                    && !bNewActor && bOrdinaryMovement && Traversals[Index].Fact.Kind == 0 && !Mobility.bCrawling
                    && !Mobility.IsStaggering(Now) && !Mobility.IsShocked(Now) && HitReactions[Index].Age > 0.5f
                    && !Health[Index].bBurning && Health[Index].DestroyedParts == 0 && CurrentSpecialFact.Phase == 0;
                // 遮挡后的远处身体保留最后一帧姿势 逻辑动画进度仍按 Mass 的当前事实推进
                BodyMesh->bUseScreenRenderStateForUpdate = bScreenPose;
                BodyMesh->VisibilityBasedAnimTickOption = bScreenPose
                    ? EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered
                    : EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
            }
            if (Behavior == EBBBMonsterBehavior::Dead && BodyMesh->bUseScreenRenderStateForUpdate)
            {
                // 死亡首次采样需要完整骨骼姿态 布娃娃仍由原有生命周期管理
                BodyMesh->bUseScreenRenderStateForUpdate = false;
                BodyMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
            }
            const bool bHadBurning = MonsterActor->HasBurningPresentation();
            const bool bCloseEnoughToBurn = Viewers.IsEmpty() || Viewers[Index].ClosestViewerDistanceSq <= FMath::Square(2400.0f);
            bool bWantBurning = Health[Index].bBurning
                && !MonsterActor->IsHidden() && BodyMesh->WasRecentlyRendered(0.35f)
                && BodyMesh->GetPredictedLODLevel() <= 2 && bCloseEnoughToBurn;
            if (bWantBurning && !bHadBurning && ActiveBurningVfx >= MaxActiveBurningVfx)
            {
                bWantBurning = false;
            }
            MonsterActor->SetBurningPresentation(bWantBurning);
            const bool bNowBurning = MonsterActor->HasBurningPresentation();
            if (bNowBurning && !bHadBurning)
            {
                ++ActiveBurningVfx;
            }
            else if (!bNowBurning && bHadBurning)
            {
                ActiveBurningVfx = FMath::Max(0, ActiveBurningVfx - 1);
            }
            if (Definition && Definition->Presentation->Gore)
            {
                auto *Mesh = MonsterActor->GetMonsterMesh();
                if (PreparationBudget > 0 && Mesh->WasRecentlyRendered(0.2f) && Mesh->GetPredictedLODLevel() <= 2
                    && MonsterActor->GetMonsterGore()->PrepareNext(*Definition->Presentation->Gore, GoreStates[Index], false))
                {
                    --PreparationBudget;
                }
                MonsterActor->GetMonsterGore()->Apply(
                    *Definition->Presentation->Gore, GoreStates[Index],
                    HitReactions[Index],
                    PresentationState.State == EBBBMonsterBehavior::Dead
                        ? DeathStates[Index].InitialVelocity : Velocities[Index].Value,
                    Variations[Index].Seed,
                    bNewActor);
            }
            Presentation->ApplyTraversalFact(
                Traversals[Index].Fact, Definition->Presentation);
            Presentation->ApplyVariationFacts(Variations[Index], PresentationState.LoopPhase);
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
            const bool bShocked = Mobility.IsShocked(Now);
            const float ShockAge = Now - Mobility.ShockStartedAt;
            const uint32 VariationSeed = Variations[Index].Seed;
            bool bShowStagger = Mobility.IsStaggering(Now);
            float StaggerProgress = Mobility.StaggerEndsAt > Mobility.StaggerStartedAt
                ? FMath::Clamp((Now - Mobility.StaggerStartedAt) / (Mobility.StaggerEndsAt - Mobility.StaggerStartedAt), 0.0f, 1.0f) : 0.0f;

            if (bShocked && bShowStagger)
            {
                const float RecoilDuration = FMath::Min(FBBBMonsterMobilityFragment::ShockRecoilSeconds,
                    Mobility.ShockEndsAt - Mobility.ShockStartedAt);
                const FRandomStream RecoilRandom(static_cast<int32>(VariationSeed ^ 0x243f6a88u));
                const float MaxRecoilDelay = 0.08f * RecoilDuration / FBBBMonsterMobilityFragment::ShockRecoilSeconds;
                const float RecoilDelay = RecoilRandom.FRandRange(0.0f, MaxRecoilDelay);
                bShowStagger = ShockAge >= RecoilDelay;

                // 在剩余起手时间内采样到原来的结束姿势
                const float SampleAge = (ShockAge - RecoilDelay) * (RecoilDuration / (RecoilDuration - RecoilDelay));
                StaggerProgress = FMath::Clamp(SampleAge / 2.3333333f, 0.0f, 1.0f);
            }

            Presentation->ApplyStaggerState(bShowStagger, StaggerProgress,
                bShocked ? EBBBMonsterHitRegion::Head : Mobility.StaggerRegion);
            Presentation->ApplyHitReaction(HitReactions[Index]);
            if (auto* Reaction = MonsterActor->FindComponentByClass<UBBBMonsterHitReactionComponent>())
            {
                const float PulseAge = ShockAge - (Mobility.bCrawling ? 0.0f : FBBBMonsterMobilityFragment::ShockRecoilSeconds);
                Reaction->ApplyShockPose(Mobility.ShockStartedAt, PulseAge,
                    bShocked ? Mobility.ShockEndsAt - Now : 0.0f, Mobility.bCrawling, VariationSeed);
            }
            if (Definition && PresentationState.State == EBBBMonsterBehavior::Dead && !Presentation->IsCorpseActive())
            {
                const float Age = Now - PresentationState.StateEnteredTime;
                const bool bSimulate = ActiveCorpses < 20 && CorpseActivationsRemaining > 0
                    && Age < Definition->Presentation->Animation.DeathAnimationDuration;
                if ((bSimulate || Age >= Definition->Presentation->Animation.DeathAnimationDuration) &&
                    Presentation->BeginCorpsePresentation(DeathStates[Index].InitialVelocity,
                        HitReactions[Index], Now, bSimulate))
                {
                    if (Presentation->IsCorpseSimulating())
                    {
                        ++ActiveCorpses;
                        --CorpseActivationsRemaining;
                    }
                    const uint8 LostParts = ChunkContext.GetFragmentView<FBBBMonsterHealthFragment>()[Index].DestroyedParts;
                    if (Definition->Presentation->Gore)
                    {
                        const auto& Wounds = GoreStates[Index].DismembermentPresets;
                        for (int32 WoundIndex = 0; WoundIndex < 5; ++WoundIndex)
                        {
                            const auto* Part = Definition->Presentation->Gore->Dismemberment->Find(Wounds[WoundIndex]);
                            if (Part && Part->DetachedSkin
                                && MonsterActor->GetMonsterMesh()->IsPhysicsStateCreated()
                                && MonsterActor->GetMonsterMesh()->bHasValidBodies
                                && (LostParts & (1u << static_cast<uint8>(Part->Region))) != 0)
                            {
                                MonsterActor->GetMonsterMesh()->SetAllBodiesBelowPhysicsDisabled(Part->Bone, true);
                            }
                        }
                    }
                }
            }
            if (auto* SoundPresentation = MonsterActor->GetMonsterSoundPresentation())
            {
                // 单机没有网络身份 声音使用世界内完整代际句柄 不创建网络事实
                const FMassEntityHandle Entity = ChunkContext.GetEntity(Index);
                const FGuid SoundInstance = bStandalone
                    ? FGuid(WorldIdentity, static_cast<uint32>(Entity.Index), static_cast<uint32>(Entity.SerialNumber), 1u)
                    : NetworkStates[Index].InstanceId;
                if (!SoundInstance.IsValid())
                {
                    continue;
                }

                EBBBMonsterBehavior VoiceState = PresentationState.State;
                uint32 VoiceAction = PresentationState.ActionId;
                float VoiceProgress = PresentationState.ActionProgress;
                if (PresentationState.State != EBBBMonsterBehavior::Dead && !Specials.IsEmpty())
                {
                    const auto& Special = Specials[Index].Fact;
                    if (Special.Phase == 1)
                    {
                        VoiceState = EBBBMonsterBehavior::Alert;
                    }
                    if (Special.Phase == 4)
                    {
                        VoiceState = EBBBMonsterBehavior::Attack;
                        VoiceAction = Special.Serial ^ 0x80000000u;
                        VoiceProgress = FMath::Clamp((Now - Special.StartedAt) / FMath::Max(Special.Duration, 0.01f), 0.0f, 1.0f);
                    }
                }
                SoundPresentation->ApplyFacts(
                    SoundInstance,
                    Definition ? Definition->Presentation->Sounds.Get() : nullptr,
                    VoiceState, VoiceAction, VoiceProgress,
                    HitReactions[Index],
                    Mobility.bCrawling, Velocities[Index].Value.Size2D(), Now, bNewActor);
                SoundPresentation->ApplyContactFacts(*MonsterActor->GetMonsterContactAudio(),
                    Grounds[Index].bGrounded,
                    Mobility.bCrawling, Velocities[Index].Value.Size2D(), Now, bNewActor);
            }
        }
    });
}
