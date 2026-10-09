#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationStateProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterCombatFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Combat/BBBMonsterCombatProcessor.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterInitializationPendingTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterVariationDefinition.h"

UBBBMonsterPresentationStateProcessor::UBBBMonsterPresentationStateProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Presentation;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Network);
    bRequiresGameThreadExecution = true;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterCombatProcessor::StaticClass()->GetFName());
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UBBBMonsterPresentationStateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FBBBMonsterCombatFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPresentationStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterVariationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    MonsterQuery.AddTagRequirement<FBBBMonsterInitializationPendingTag>(EMassFragmentPresence::None);
}

void UBBBMonsterPresentationStateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    if (!ensureMsgf(Context.GetWorld() != nullptr, TEXT("[UBBBM]Presentation snapshot requires a valid world")))
    {
        return;
    }

    const float WorldTime = Context.GetWorld()->GetTimeSeconds();
    const float DeltaTime = FMath::Max(Context.GetDeltaTimeSeconds(), 0.0f);
    MonsterQuery.ForEachEntityChunk(Context, [WorldTime, DeltaTime](FMassExecutionContext& ChunkContext)
    {
        // 把权威状态转换为表现层可安全读取的快照
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FBBBMonsterBehaviorFragment> States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        TArrayView<FBBBMonsterPresentationStateFragment> PresentationStates = ChunkContext.GetMutableFragmentView<FBBBMonsterPresentationStateFragment>();

        const auto Combats = ChunkContext.GetFragmentView<FBBBMonsterCombatFragment>();
        const auto Healths = ChunkContext.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Variations = ChunkContext.GetFragmentView<FBBBMonsterVariationFragment>();
        const auto Networks = ChunkContext.GetFragmentView<FBBBMonsterNetworkFragment>();

        // 逐个实体复制状态快照数据
        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FBBBMonsterPresentationStateFragment& PresentationState = PresentationStates[Index];
            const auto& Variation = Variations[Index];
            if (PresentationState.LoopSeed != Variation.Seed)
            {
                PresentationState.LoopSeed = Variation.Seed;
                PresentationState.LoopPhase = Variation.PhaseOffset;
            }
            const float NormalizedSpeed = Velocities[Index].Value.Size2D() / FMath::Max(Variation.SpeedScale, 0.01f);
            const UBBBMonsterDefinition* Definition = Networks[Index].Definition.Get();
            if (!ensureMsgf(Definition && Definition->Variation && Variation.LocomotionStyle < 3,
                TEXT("[BBBMonsterVariation]Initialized snapshot requires cycle configuration")))
            {
                continue;
            }
            const UBBBMonsterVariationDefinition& Settings = *Definition->Variation;
            const uint32 IdleIndex = Variation.Seed % 3u;
            float Duration = static_cast<float>(Settings.IdleCycleSeconds[IdleIndex]);
            if (States[Index].State == EBBBMonsterBehavior::Alert)
            {
                Duration = Settings.AlertCycleSeconds;
            }
            if (States[Index].State == EBBBMonsterBehavior::Patrol || States[Index].State == EBBBMonsterBehavior::Chase)
            {
                const FVector& Cycle = Settings.MovementCycleSeconds[Variation.LocomotionStyle];
                Duration = static_cast<float>(Settings.IdleCycleSeconds[Variation.LocomotionStyle]);
                if (NormalizedSpeed <= Definition->WalkSpeed)
                {
                    Duration = FMath::Lerp(Duration, static_cast<float>(Cycle.X), FMath::Clamp(NormalizedSpeed / Definition->WalkSpeed, 0.0f, 1.0f));
                }
                if (NormalizedSpeed > Definition->WalkSpeed && NormalizedSpeed <= Definition->RunSpeed)
                {
                    Duration = FMath::Lerp(static_cast<float>(Cycle.X), static_cast<float>(Cycle.Y),
                        FMath::Clamp((NormalizedSpeed - Definition->WalkSpeed) / (Definition->RunSpeed - Definition->WalkSpeed), 0.0f, 1.0f));
                }
                if (NormalizedSpeed > Definition->RunSpeed)
                {
                    Duration = FMath::Lerp(static_cast<float>(Cycle.Y), static_cast<float>(Cycle.Z),
                        FMath::Clamp((NormalizedSpeed - Definition->RunSpeed) / (Definition->SprintSpeed - Definition->RunSpeed), 0.0f, 1.0f));
                }
            }
            const bool bMovingLoop = States[Index].State == EBBBMonsterBehavior::Patrol || States[Index].State == EBBBMonsterBehavior::Chase;
            const float Frequency = (bMovingLoop ? Variation.SpeedScale : 1.0f) / FMath::Max(Duration, 0.01f);
            PresentationState.LoopPhase = FMath::Frac(PresentationState.LoopPhase + DeltaTime * Frequency);
            PresentationState.State = States[Index].State;
            PresentationState.Speed = Velocities[Index].Value.Size2D();
            PresentationState.StateEnteredTime = States[Index].StateEnteredTime;
            PresentationState.ActionId = States[Index].ActionId;
            PresentationState.ActionProgress = 0.0f;
            const float Elapsed = FMath::Max(WorldTime - States[Index].StateEnteredTime, 0.0f);

            // 分段映射让动画命中帧与逻辑前摇结束严格对齐
            if (States[Index].State == EBBBMonsterBehavior::Attack)
            {
                const FBBBMonsterCombatFragment& Combat = Combats[Index];
                PresentationState.ActionProgress = Elapsed <= Combat.AttackWindup
                    ? Combat.AnimationHitFraction * Elapsed / Combat.AttackWindup
                    : Combat.AnimationHitFraction + (1.0f - Combat.AnimationHitFraction) * (Elapsed - Combat.AttackWindup) / Combat.AttackRecovery;
            }

            if (States[Index].State == EBBBMonsterBehavior::Dead)
            {
                PresentationState.ActionProgress = Elapsed / Healths[Index].DeathLifetime;
            }

            PresentationState.ActionProgress = FMath::Clamp(PresentationState.ActionProgress, 0.0f, 1.0f);
        }
    });
}
