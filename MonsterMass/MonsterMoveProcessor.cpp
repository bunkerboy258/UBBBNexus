#include "BBBWork/UBBBNexus/MonsterMass/MonsterMoveProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterMassFragments.h"

UMonsterMoveProcessor::UMonsterMoveProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
}

void UMonsterMoveProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterMoveProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    const float DeltaTime = Context.GetDeltaTimeSeconds();

    if (!ensureMsgf(DeltaTime >= 0.0f, TEXT("[UBBBM]Delta time must not be negative")))
    {
        return;
    }

    MonsterQuery.ForEachEntityChunk(Context, [DeltaTime](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        const TConstArrayView<FMonsterMovementFragment> Movements = ChunkContext.GetFragmentView<FMonsterMovementFragment>();
        const TConstArrayView<FMonsterTargetFragment> Targets = ChunkContext.GetFragmentView<FMonsterTargetFragment>();
        TArrayView<FMonsterStateFragment> States = ChunkContext.GetMutableFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            const FMonsterMovementFragment& Movement = Movements[Index];
            const FVector TargetLocation = Targets[Index].TargetLocation;
            FVector Position = Transform.GetLocation();
            FVector ToTarget = TargetLocation - Position;
            ToTarget.Z = 0.0f;

            const float StopRadius = FMath::Max(Movement.StopRadius, 0.0f);
            const float DistanceSquared = ToTarget.SizeSquared();
            const float StopRadiusSquared = FMath::Square(StopRadius);

            if (DistanceSquared <= StopRadiusSquared)
            {
                States[Index].State = EMonsterState::Attack;
                continue;
            }

            States[Index].State = EMonsterState::Chase;

            const float Distance = FMath::Sqrt(DistanceSquared);
            const FVector Direction = ToTarget / Distance;
            const float MoveDistance = FMath::Min(FMath::Max(Movement.MoveSpeed, 0.0f) * DeltaTime, Distance - StopRadius);
            Position += Direction * MoveDistance;
            Transform.SetLocation(Position);
        }
    });
}
