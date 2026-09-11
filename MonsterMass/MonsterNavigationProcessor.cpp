#include "BBBWork/UBBBNexus/MonsterMass/MonsterNavigationProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterPerceptionProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

UMonsterNavigationProcessor::UMonsterNavigationProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UMonsterPerceptionProcessor::StaticClass()->GetFName());
}

void UMonsterNavigationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterTargetRequestFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMonsterMovementFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterNavigationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster navigation requires a valid world")))
    {
        return;
    }

    UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

    if (!ensureMsgf(NavigationSystem != nullptr, TEXT("[UBBBM]Monster navigation requires NavigationSystem")))
    {
        return;
    }

    const float DeltaTime = Context.GetDeltaTimeSeconds();

    if (!ensureMsgf(DeltaTime >= 0.0f, TEXT("[UBBBM]Monster navigation delta time must not be negative")))
    {
        return;
    }

    const float WorldTime = World->GetTimeSeconds();

    MonsterQuery.ForEachEntityChunk(Context, [DeltaTime, NavigationSystem, World, WorldTime](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        TArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetMutableFragmentView<FMassVelocityFragment>();
        const TConstArrayView<FMonsterTargetRequestFragment> Targets = ChunkContext.GetFragmentView<FMonsterTargetRequestFragment>();
        TArrayView<FMonsterMovementFragment> Movements = ChunkContext.GetMutableFragmentView<FMonsterMovementFragment>();
        TArrayView<FMonsterStateFragment> States = ChunkContext.GetMutableFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            FMassVelocityFragment& Velocity = Velocities[Index];
            const FMonsterTargetRequestFragment& Target = Targets[Index];
            FMonsterMovementFragment& Movement = Movements[Index];
            FMonsterStateFragment& State = States[Index];

            if (!Target.bHasTarget || State.State == EMonsterState::Dead || State.State == EMonsterState::Hurt)
            {
                Velocity.Value = FVector::ZeroVector;
                continue;
            }

            const FVector CurrentLocation = Transform.GetLocation();
            FVector ToTarget = Target.TargetLocation - CurrentLocation;
            ToTarget.Z = 0.0f;

            const float StopRadius = FMath::Max(Movement.StopRadius, 0.0f);

            if (ToTarget.SizeSquared() <= FMath::Square(StopRadius))
            {
                State.State = EMonsterState::Attack;
                State.StateEnteredTime = WorldTime;
                Velocity.Value = FVector::ZeroVector;
                continue;
            }

            if (WorldTime >= Movement.NextPathRefreshTime)
            {
                const UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(
                    World,
                    CurrentLocation,
                    Target.TargetLocation);

                if (Path != nullptr && Path->PathPoints.Num() > 1)
                {
                    Movement.NextPathPoint = Path->PathPoints[1];
                }
                else
                {
                    Movement.NextPathPoint = Target.TargetLocation;
                }

                Movement.NextPathRefreshTime = WorldTime + 0.25f;
            }

            FVector ToPathPoint = Movement.NextPathPoint - CurrentLocation;
            ToPathPoint.Z = 0.0f;

            if (ToPathPoint.IsNearlyZero())
            {
                Velocity.Value = FVector::ZeroVector;
                continue;
            }

            const FVector PathDirection = ToPathPoint.GetSafeNormal();
            const float MoveSpeed = FMath::Max(Movement.MoveSpeed, 0.0f);
            const float MoveDistance = FMath::Min(MoveSpeed * DeltaTime, ToPathPoint.Size());
            const FVector NewLocation = CurrentLocation + PathDirection * MoveDistance;

            Transform.SetLocation(NewLocation);
            Transform.SetRotation(PathDirection.ToOrientationQuat());
            Velocity.Value = PathDirection * MoveSpeed;
            State.State = EMonsterState::Chase;
        }
    });
}
