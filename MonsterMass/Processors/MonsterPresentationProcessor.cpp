#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterPresentationProcessor.h"

#include "MassActorSubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterBasicActor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterPresentationProcessor::UMonsterPresentationProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UMonsterAvoidanceProcessor::StaticClass()->GetFName());
}

void UMonsterPresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterPresentationStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterPresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    MonsterQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& ChunkContext)
    {
        // 表现层只读取逻辑结果，不参与决策
        TArrayView<FMassActorFragment> Actors = ChunkContext.GetMutableFragmentView<FMassActorFragment>();
        const TConstArrayView<FTransformFragment> Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FMonsterAvoidanceFragment> Avoidances = ChunkContext.GetFragmentView<FMonsterAvoidanceFragment>();
        const TConstArrayView<FMonsterPresentationStateFragment> PresentationStates = ChunkContext.GetFragmentView<FMonsterPresentationStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            // 取得实体对应的表现演员
            AMonsterBasicActor* MonsterActor = Cast<AMonsterBasicActor>(Actors[Index].GetMutable());

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
            MonsterActor->SetCollisionRadius(Avoidances[Index].CollisionRadius);

            // 取得表现组件同步状态和移动速度
            UMonsterPresentationComponent* Presentation = MonsterActor->GetMonsterPresentation();

            if (!ensureMsgf(Presentation != nullptr, TEXT("[UBBBM]Monster actor requires MonsterPresentationComponent")))
            {
                continue;
            }

            const FMonsterPresentationStateFragment& PresentationState = PresentationStates[Index];
            // 将状态和速度交给表现组件选择动画
            Presentation->ApplyPresentationState(
                PresentationState.State,
                Velocities[Index].Value.Size2D(),
                PresentationState.StateEnteredTime);
        }
    });
}
