#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterPresentationStateFragment.h"
#include "MassActorSubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterPresentationStateProcessor.h"

UBBBMonsterPresentationProcessor::UBBBMonsterPresentationProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Presentation;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterPresentationStateProcessor::StaticClass()->GetFName());
}

void UBBBMonsterPresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterPresentationStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterPresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    MonsterQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& ChunkContext)
    {
        // 表现层只读取逻辑结果，不参与决策
        TArrayView<FMassActorFragment> Actors = ChunkContext.GetMutableFragmentView<FMassActorFragment>();
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FBBBMonsterAvoidanceFragment> Avoidances = ChunkContext.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const TConstArrayView<FBBBMonsterPresentationStateFragment> PresentationStates = ChunkContext.GetFragmentView<FBBBMonsterPresentationStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            // 取得实体对应的表现演员
            ABBBMonsterPresentationActor* MonsterActor = Cast<ABBBMonsterPresentationActor>(Actors[Index].GetMutable());

            if (!IsValid(MonsterActor))
            {
                continue;
            }

            const FTransform& MonsterTransform = Transforms[Index].GetTransform();
            // 同步实体位置和朝向到骨骼表现 Actor
            MonsterActor->SetActorLocationAndRotation(
                MonsterTransform.GetLocation(),
                MonsterTransform.GetRotation(),
                false,
                nullptr,
                ETeleportType::TeleportPhysics);
            MonsterActor->SetActorEnableCollision(false);

            // 取得表现组件同步状态和移动速度
            UBBBMonsterPresentationComponent* Presentation = MonsterActor->GetMonsterPresentation();

            if (!ensureMsgf(Presentation != nullptr, TEXT("[UBBBM]Monster actor requires BBBMonsterPresentationComponent")))
            {
                continue;
            }

            const FBBBMonsterPresentationStateFragment& PresentationState = PresentationStates[Index];
            // 将状态和速度交给表现组件选择动画
            Presentation->ApplyPresentationState(
                PresentationState.State,
                Velocities[Index].Value.Size2D(),
                PresentationState.StateEnteredTime,
                PresentationState.ActionId,
                PresentationState.ActionProgress);
        }
    });
}
