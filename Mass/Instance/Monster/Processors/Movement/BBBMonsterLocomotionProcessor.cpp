#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassExecutionContext.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

UBBBMonsterLocomotionProcessor::UBBBMonsterLocomotionProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterAvoidanceProcessor::StaticClass()->GetFName());
    bRequiresGameThreadExecution = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
}

EBBBMonsterGait UBBBMonsterLocomotionProcessor::SelectGait(const FBBBMonsterMovementFragment& Movement, const float RemainingDistance, const bool bPatrol)
{
    if (bPatrol || RemainingDistance <= Movement.WalkDistance)
    {
        return EBBBMonsterGait::Walk;
    }
    if (Movement.Gait == EBBBMonsterGait::Walk && RemainingDistance < Movement.WalkDistance + Movement.GaitHysteresis)
    {
        return EBBBMonsterGait::Walk;
    }
    if (RemainingDistance >= Movement.SprintDistance)
    {
        return EBBBMonsterGait::Sprint;
    }
    if (Movement.Gait == EBBBMonsterGait::Sprint && RemainingDistance > Movement.SprintDistance - Movement.GaitHysteresis)
    {
        return EBBBMonsterGait::Sprint;
    }
    return EBBBMonsterGait::Run;
}

float UBBBMonsterLocomotionProcessor::CalculateSpeed(const FBBBMonsterMovementFragment& Movement, const float CurrentSpeed, const float TargetSpeed, const float RemainingDistance, const float DeltaSeconds)
{
    if (DeltaSeconds <= 0.0f || RemainingDistance <= 0.0f)
    {
        return 0.0f;
    }
    const float BrakingSpeed = FMath::Sqrt(2.0f * Movement.Deceleration * RemainingDistance);
    const float DesiredSpeed = FMath::Min(TargetSpeed, BrakingSpeed);
    const float Rate = DesiredSpeed > CurrentSpeed ? Movement.Acceleration : Movement.Deceleration;
    const float Speed = FMath::FInterpConstantTo(CurrentSpeed, DesiredSpeed, DeltaSeconds, Rate);
    return FMath::Clamp(Speed, 0.0f, FMath::Min(BrakingSpeed, Movement.SprintSpeed));
}

void UBBBMonsterLocomotionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterLocomotionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    const float DeltaSeconds = Context.GetDeltaTimeSeconds();
    if (!ensureMsgf(World && FMath::IsFinite(DeltaSeconds) && DeltaSeconds >= 0.0f, TEXT("[UBBBM]Locomotion requires a world and valid time step")))
    {
        return;
    }
    MonsterQuery.ForEachEntityChunk(Context, [World, DeltaSeconds](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Velocities = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        auto Movements = Chunk.GetMutableFragmentView<FBBBMonsterMovementFragment>();
        const auto Navigation = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Avoidance = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            FVector& Velocity = Velocities[Index].Value;
            FBBBMonsterMovementFragment& Movement = Movements[Index];
            const FBBBMonsterNavigationFragment& Path = Navigation[Index];
            const bool bPatrol = States[Index].State == EBBBMonsterBehavior::Patrol;
            const bool bChase = States[Index].State == EBBBMonsterBehavior::Chase;
            if ((!bPatrol && !bChase) || DeltaSeconds <= SMALL_NUMBER || !Path.bHasPath ||
                !Path.PathPoints.IsValidIndex(Path.PathPointIndex) || !Path.TailDistances.IsValidIndex(Path.PathPointIndex))
            {
                Velocity = FVector::ZeroVector;
                Movement.Gait = EBBBMonsterGait::Walk;
                continue;
            }

            const FVector Location = Transform.GetLocation();
            const FVector ToPoint = Path.PathPoints[Path.PathPointIndex] - Location;
            const float PointDistance = ToPoint.Size2D();
            const float StopRadius = bPatrol ? 15.0f : Movement.StopRadius;
            const float Remaining = FMath::Max(PointDistance + Path.TailDistances[Path.PathPointIndex] - StopRadius, 0.0f);
            const EBBBMonsterGait PreviousGait = Movement.Gait;
            Movement.Gait = SelectGait(Movement, Remaining, bPatrol);
            const float TargetSpeed = Movement.Gait == EBBBMonsterGait::Walk ? Movement.WalkSpeed :
                Movement.Gait == EBBBMonsterGait::Run ? Movement.RunSpeed : Movement.SprintSpeed;
            const float Speed = CalculateSpeed(Movement, Velocity.Size2D(), TargetSpeed, Remaining, DeltaSeconds);
            const float Distance = FMath::Min3(Speed * DeltaSeconds, PointDistance, Remaining);
            if (Distance <= SMALL_NUMBER || PointDistance <= SMALL_NUMBER)
            {
                Velocity = FVector::ZeroVector;
                continue;
            }

            const FBBBMonsterAvoidanceFragment& Neighbors = Avoidance[Index];
            const float Steering = FMath::Clamp(Neighbors.SeparationStrength * Neighbors.AvoidanceWeight, 0.0f, 0.65f);
            const FVector Direction = (ToPoint.GetSafeNormal2D() + Neighbors.SeparationDirection * Steering).GetSafeNormal2D();
            FVector Destination = Location + Direction * Distance;
            Destination.Z = FMath::Lerp(Location.Z, Path.PathPoints[Path.PathPointIndex].Z + Path.HeightOffset, Distance / PointDistance);

            // 同一求解内约束导航边界 防止避让把实体推到墙外
            FVector HitLocation = Destination;
            if (UNavigationSystemV1::NavigationRaycast(World, Location, Destination, HitLocation))
            {
                Destination = HitLocation;
            }
            if (Destination.ContainsNaN() || FVector::Dist2D(Location, Destination) > Distance + 1.0f)
            {
                ensureMsgf(false, TEXT("[UBBBM]Navigation constraint produced an invalid movement"));
                Velocity = FVector::ZeroVector;
                continue;
            }

            Transform.SetLocation(Destination);
            Velocity = (Destination - Location) / DeltaSeconds;
            if (!Velocity.IsNearlyZero())
            {
                Transform.SetRotation(Velocity.ToOrientationQuat());
            }
            if (PreviousGait != Movement.Gait)
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Gait Entity=%d Gait=%d Speed=%.1f Remaining=%.1f"),
                    Chunk.GetEntity(Index).Index, static_cast<int32>(Movement.Gait), Velocity.Size2D(), Remaining);
            }
        }
    });
}
