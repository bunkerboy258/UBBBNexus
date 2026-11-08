#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationStateProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
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
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
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
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
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
        const auto Specials = ChunkContext.GetFragmentView<FBBBMonsterSpecialFragment>();
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
            if (!ensureMsgf(Definition && Definition->Presentation && Variation.LocomotionStyle < 3,
                TEXT("[BBBMonsterVariation]Initialized snapshot requires cycle configuration")))
            {
                continue;
            }
            const auto& Settings = Definition->Presentation->Animation;
            int32 Style = Variation.LocomotionStyle;
            if (!Specials.IsEmpty() && States[Index].State == EBBBMonsterBehavior::Chase && Specials[Index].Fact.Phase >= 5)
            {
                Style = Specials[Index].Fact.Phase == 5 ? 1 : 2;
            }
            float Duration = static_cast<float>(Settings.IdleCycleSeconds[Style]);
            float ReferenceSpeed = 0.0f;
            if (States[Index].State == EBBBMonsterBehavior::Alert)
            {
                Duration = Settings.AlertCycleSeconds;
            }
            if (States[Index].State == EBBBMonsterBehavior::Patrol)
            {
                ReferenceSpeed = Definition->PatrolSpeed[Style];
                Duration = FMath::Lerp(static_cast<float>(Settings.IdleCycleSeconds[Style]),
                    static_cast<float>(Settings.PatrolCycleSeconds[Style]),
                    FMath::Clamp(NormalizedSpeed / Definition->PatrolSpeed[Style], 0.0f, 1.0f));
            }
            if (States[Index].State == EBBBMonsterBehavior::Chase)
            {
                ReferenceSpeed = Definition->WalkSpeed[Style];
                const FVector& Cycle = Settings.MovementCycleSeconds[Style];
                Duration = static_cast<float>(Settings.IdleCycleSeconds[Style]);
                if (NormalizedSpeed <= Settings.SlowWalkSampleSpeed[Style])
                {
                    Duration = FMath::Lerp(Duration, static_cast<float>(Settings.SlowWalkCycleSeconds[Style]),
                        FMath::Clamp(NormalizedSpeed / Settings.SlowWalkSampleSpeed[Style], 0.0f, 1.0f));
                }
                if (NormalizedSpeed > Settings.SlowWalkSampleSpeed[Style] && NormalizedSpeed <= Definition->WalkSpeed[Style])
                {
                    Duration = FMath::Lerp(static_cast<float>(Settings.SlowWalkCycleSeconds[Style]), static_cast<float>(Cycle.X),
                        FMath::Clamp((NormalizedSpeed - Settings.SlowWalkSampleSpeed[Style]) / (Definition->WalkSpeed[Style] - Settings.SlowWalkSampleSpeed[Style]), 0.0f, 1.0f));
                }
                if (NormalizedSpeed > Definition->WalkSpeed[Style] && NormalizedSpeed <= Definition->WalkerChaseWalkSpeed[Style])
                {
                    Duration = FMath::Lerp(static_cast<float>(Cycle.X), static_cast<float>(Settings.PursuitWalkCycleSeconds[Style]),
                        FMath::Clamp((NormalizedSpeed - Definition->WalkSpeed[Style]) / (Definition->WalkerChaseWalkSpeed[Style] - Definition->WalkSpeed[Style]), 0.0f, 1.0f));
                }
                if (NormalizedSpeed > Definition->WalkerChaseWalkSpeed[Style] && NormalizedSpeed <= Definition->RunSpeed[Style])
                {
                    ReferenceSpeed = FMath::Lerp(Definition->WalkSpeed[Style], Definition->RunSpeed[Style],
                        FMath::Clamp((NormalizedSpeed - Definition->WalkerChaseWalkSpeed[Style]) / (Definition->RunSpeed[Style] - Definition->WalkerChaseWalkSpeed[Style]), 0.0f, 1.0f));
                    Duration = FMath::Lerp(static_cast<float>(Settings.PursuitWalkCycleSeconds[Style]), static_cast<float>(Cycle.Y),
                        FMath::Clamp((NormalizedSpeed - Definition->WalkerChaseWalkSpeed[Style]) / (Definition->RunSpeed[Style] - Definition->WalkerChaseWalkSpeed[Style]), 0.0f, 1.0f));
                }
                if (NormalizedSpeed > Definition->RunSpeed[Style])
                {
                    ReferenceSpeed = FMath::Lerp(Definition->RunSpeed[Style], Definition->SprintSpeed[Style],
                        FMath::Clamp((NormalizedSpeed - Definition->RunSpeed[Style]) / (Definition->SprintSpeed[Style] - Definition->RunSpeed[Style]), 0.0f, 1.0f));
                    Duration = FMath::Lerp(static_cast<float>(Cycle.Y), static_cast<float>(Cycle.Z),
                        FMath::Clamp((NormalizedSpeed - Definition->RunSpeed[Style]) / (Definition->SprintSpeed[Style] - Definition->RunSpeed[Style]), 0.0f, 1.0f));
                }
            }
            const bool bMovingLoop = States[Index].State == EBBBMonsterBehavior::Patrol || States[Index].State == EBBBMonsterBehavior::Chase;
            // 循环秒数来自实际动画时长 只允许个体速度带来有限的节奏浮动
            const float MotionRate = bMovingLoop && ReferenceSpeed > 0.0f ? NormalizedSpeed / ReferenceSpeed : 1.0f;
            const float PlaybackRate = FMath::Clamp(Settings.PlaybackRate * (bMovingLoop ? Variation.SpeedScale * MotionRate : 1.0f), 0.8f, 1.2f);
            const float Frequency = PlaybackRate / FMath::Max(Duration, 0.01f);
            PresentationState.LoopPhase = FMath::Frac(PresentationState.LoopPhase + DeltaTime * Frequency);
            PresentationState.State = States[Index].State;
            PresentationState.Speed = Velocities[Index].Value.Size2D();
            PresentationState.StateEnteredTime = States[Index].StateEnteredTime;
            PresentationState.ActionId = States[Index].ActionId;
            PresentationState.ActionProgress = 0.0f;
            const float Elapsed = FMath::Max(WorldTime - States[Index].StateEnteredTime, 0.0f);

            // 按完整原始时长推进动作 不压缩命中前后的片段
            if (States[Index].State == EBBBMonsterBehavior::Attack)
            {
                const FBBBMonsterCombatFragment& Combat = Combats[Index];
                PresentationState.ActionProgress = Elapsed / (Combat.AttackWindup + Combat.AttackRecovery);
            }

            if (States[Index].State == EBBBMonsterBehavior::Dead)
            {
                PresentationState.ActionProgress = Elapsed / Definition->Presentation->Animation.DeathAnimationDuration;
            }

            PresentationState.ActionProgress = FMath::Clamp(PresentationState.ActionProgress, 0.0f, 1.0f);
        }
    });
}
