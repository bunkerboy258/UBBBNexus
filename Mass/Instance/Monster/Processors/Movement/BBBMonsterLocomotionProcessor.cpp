#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterLocomotionProcessor.h"
#include "BBBMonsterStep.h"
#include "BBBMonsterNavigation.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Combat/BBBMonsterSpecialFragment.h"
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
#include "NavMesh/RecastNavMesh.h"
#include "Engine/World.h"
#include "CollisionShape.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"
#include "BBBMonsterSteering.h"

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
    if (bPatrol)
    {
        return EBBBMonsterGait::Walk;
    }
    if (Movement.MaxGait == EBBBMonsterGait::Walk)
    {
        return Movement.bWalkerRunning ? EBBBMonsterGait::Run : EBBBMonsterGait::Walk;
    }
    if (Movement.MaxGait == EBBBMonsterGait::Sprint && (Movement.Gait == EBBBMonsterGait::Sprint || RemainingDistance >= Movement.SprintDistance))
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
    const float DesiredSpeed = FMath::Min(TargetSpeed, Movement.SprintSpeed);
    const float Rate = DesiredSpeed > CurrentSpeed ? Movement.Acceleration : Movement.Deceleration;
    const float Speed = FMath::FInterpConstantTo(CurrentSpeed, DesiredSpeed, DeltaSeconds, Rate);
    return FMath::Clamp(Speed, 0.0f, Movement.SprintSpeed);
}

FQuat UBBBMonsterLocomotionProcessor::CalculateFacing(const FQuat& CurrentRotation, const FVector& HorizontalDelta,
    const FVector& Velocity, const float DeltaSeconds, const float DegreesPerSecond)
{
    if (!ensureMsgf(!CurrentRotation.ContainsNaN() && !HorizontalDelta.ContainsNaN() && !Velocity.ContainsNaN() &&
        FMath::IsFinite(DeltaSeconds) && DeltaSeconds >= 0.0f, TEXT("[UBBBM]Facing requires finite current motion")))
    {
        return CurrentRotation;
    }

    const FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.0f);
    if (DeltaSeconds <= SMALL_NUMBER || HorizontalDelta.IsNearlyZero() ||
        HorizontalVelocity.SizeSquared() <= FMath::Square(10.0f) ||
        FVector::DotProduct(HorizontalDelta, HorizontalVelocity) <= 0.0f)
    {
        return CurrentRotation;
    }

    return TurnTowards(CurrentRotation, HorizontalDelta, DeltaSeconds, DegreesPerSecond);
}

FQuat UBBBMonsterLocomotionProcessor::TurnTowards(const FQuat& Current, const FVector& Direction, const float DeltaSeconds, const float DegreesPerSecond)
{
    if (!ensureMsgf(!Current.ContainsNaN() && !Direction.ContainsNaN() && FMath::IsFinite(DeltaSeconds)
        && FMath::IsFinite(DegreesPerSecond) && DegreesPerSecond > 0.0f, TEXT("[UBBBM]Turn requires finite direction and rate")))
    {
        return Current;
    }
    if (Direction.IsNearlyZero() || DeltaSeconds <= 0.0f)
    {
        return Current;
    }
    const float CurrentYaw = Current.Rotator().Yaw;
    const float TargetYaw = Direction.Rotation().Yaw;
    const float DeltaYaw = FMath::Clamp(FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw),
        -DegreesPerSecond * DeltaSeconds, DegreesPerSecond * DeltaSeconds);
    return FRotator(0.0f, CurrentYaw + DeltaYaw, 0.0f).Quaternion();
}

void UBBBMonsterLocomotionProcessor::SolveGroundMotion(UWorld& World, const FBBBMonsterMovementFragment& Movement,
    FBBBMonsterGroundFragment& Ground, FVector& Location, FVector& Velocity,
    const FVector& HorizontalDelta, const float DeltaSeconds)
{
    Ground.RequestedDistance = 0.0f;
    Ground.MovedDistance = 0.0f;
    if (!ensureMsgf(FMath::IsFinite(DeltaSeconds) && DeltaSeconds >= 0.0f && !Location.ContainsNaN() &&
        !Velocity.ContainsNaN() && !HorizontalDelta.ContainsNaN() && Movement.CapsuleRadius > 0.0f &&
        Movement.CapsuleHalfHeight >= Movement.CapsuleRadius && Movement.WalkableFloorZ > 0.0f,
        TEXT("[UBBBM]Ground solver requires finite motion and a valid capsule")))
    {
        return;
    }
    if (DeltaSeconds <= SMALL_NUMBER)
    {
        return;
    }

    constexpr float Skin = 0.5f;
    constexpr float SupportDistance = 3.0f;
    const FCollisionShape Capsule = FCollisionShape::MakeCapsule(Movement.CapsuleRadius, Movement.CapsuleHalfHeight);
    const float SupportShrink = FMath::Min(Skin, Movement.CapsuleRadius * 0.1f);
    const FCollisionShape SupportCapsule = FCollisionShape::MakeCapsule(Movement.CapsuleRadius - SupportShrink, Movement.CapsuleHalfHeight - SupportShrink);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterGround), false);
    const auto Sweep = [&World, &Capsule, &Params](const FVector& Start, const FVector& End, FHitResult& Hit)
    {
        return World.SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Pawn, Capsule, Params);
    };
    const auto Walkable = [&Movement](const FHitResult& Hit)
    {
        return Hit.IsValidBlockingHit() && Hit.ImpactNormal.Z >= Movement.WalkableFloorZ;
    };
    const auto FindSupport = [&World, &Movement, &SupportCapsule, &Params, &Walkable, SupportShrink](FVector& Center, const float Distance, FVector& Normal, float* SurfaceHeight = nullptr)
    {
        FHitResult Floor;
        // 支撑查询略缩胶囊 避免贴墙的零时刻侧面命中遮住脚下地面
        if (World.SweepSingleByChannel(Floor, Center + FVector::UpVector * Skin,
            Center - FVector::UpVector * (Distance + SupportShrink), FQuat::Identity, ECC_Pawn, SupportCapsule, Params) &&
            Walkable(Floor) && Floor.Normal.Z > KINDA_SMALL_NUMBER)
        {
            FHitResult CenterFloor;
            if (Floor.ImpactNormal.Z > 0.99f &&
                Floor.ImpactPoint.Z > Center.Z - Movement.CapsuleHalfHeight + Movement.MaxStepHeight + Skin &&
                World.LineTraceSingleByChannel(CenterFloor, Center,
                Center - FVector::UpVector * (Movement.CapsuleHalfHeight + Distance + Movement.MaxStepHeight), ECC_Pawn, Params) &&
                Walkable(CenterFloor) && CenterFloor.ImpactNormal.Z > 0.99f &&
                Floor.ImpactPoint.Z > CenterFloor.ImpactPoint.Z + Movement.MaxStepHeight + Skin)
            {
                Center.Z = CenterFloor.ImpactPoint.Z + Movement.CapsuleHalfHeight + Skin;
                Normal = CenterFloor.ImpactNormal;
                if (SurfaceHeight)
                {
                    *SurfaceHeight = Floor.ImpactPoint.Z;
                }
                return true;
            }
            Center.Z = Floor.Location.Z + Skin + SupportShrink / Floor.Normal.Z;
            Normal = Floor.ImpactNormal;
            if (SurfaceHeight)
            {
                *SurfaceHeight = Floor.ImpactPoint.Z;
            }
            return true;
        }
        return false;
    };

    // 出生或外部支撑移动造成的浅层重叠只在此处消解 不另建位置写入者
    if (!Ground.bGrounded || Velocity.Z > 0.0f || !HorizontalDelta.IsNearlyZero() ||
        World.OverlapBlockingTestByChannel(Location, FQuat::Identity, ECC_Pawn, Capsule, Params))
    {
        for (int32 Attempt = 0; Attempt < 4; ++Attempt)
        {
            FHitResult Penetration;
            if (!Sweep(Location, Location, Penetration) || !Penetration.bStartPenetrating)
            {
                break;
            }
            Location += Penetration.Normal * (Penetration.PenetrationDepth + Skin);
        }
    }

    // 静止活体仍逐帧检测真实支撑 失败结果在本帧首个子步直接使用
    const bool bCheckedStationarySupport = Ground.bGrounded && Velocity.Z <= 0.0f && HorizontalDelta.IsNearlyZero();
    if (bCheckedStationarySupport)
    {
        Ground.bGrounded = FindSupport(Location, SupportDistance, Ground.SupportNormal);
        if (Ground.bGrounded)
        {
            Velocity = FVector::ZeroVector;
            Ground.UnsimulatedSeconds = 0.0f;
            return;
        }
    }

    /** 长帧仅保留有限欠账 单轮至多八步 不用大步长穿越障碍 */
    constexpr int32 MaximumSteps = 8;
    constexpr float FixedStep = 1.0f / 60.0f;
    const float AvailableSeconds = DeltaSeconds + Ground.UnsimulatedSeconds;
    const float SimulationSeconds = FMath::Min(AvailableSeconds, MaximumSteps * FixedStep);
    Ground.UnsimulatedSeconds = FMath::Clamp(AvailableSeconds - SimulationSeconds, 0.0f, 0.25f);
    const int32 Steps = FMath::Clamp(FMath::CeilToInt(SimulationSeconds / FixedStep), 1, MaximumSteps);
    const float StepSeconds = SimulationSeconds / Steps;
    const FVector BoundedHorizontal = HorizontalDelta * FMath::Min(1.0f, SimulationSeconds / DeltaSeconds);
    Ground.RequestedDistance = BoundedHorizontal.Size2D();
    const FVector HorizontalStep = FVector(BoundedHorizontal.X, BoundedHorizontal.Y, 0.0f) / Steps;
    const FVector FrameStart = Location;
    for (int32 Step = 0; Step < Steps; ++Step)
    {
        // 后续子步沿用上一子步末尾的支撑结果 两次查询之间位置没有再次变化
        if (Step == 0 && !bCheckedStationarySupport)
        {
            Ground.bGrounded = Velocity.Z <= 0.0f && FindSupport(Location, SupportDistance, Ground.SupportNormal);
        }
        const bool bWasGrounded = Ground.bGrounded;
        FVector Delta = HorizontalStep;
        if (bWasGrounded)
        {
            Velocity.Z = 0.0f;
            Delta.Z = -FVector::DotProduct(HorizontalStep, Ground.SupportNormal) / Ground.SupportNormal.Z;
        }
        if (!bWasGrounded)
        {
            Ground.SupportNormal = FVector::UpVector;
            Delta.Z = Velocity.Z * StepSeconds + 0.5f * World.GetGravityZ() * StepSeconds * StepSeconds;
            Velocity.Z += World.GetGravityZ() * StepSeconds;
        }

        const FVector Start = Location;
        FHitResult Hit;
        const bool bBlocked = Sweep(Start, Start + Delta, Hit);
        if (bBlocked)
        {
            UE_LOG(LogTemp, VeryVerbose, TEXT("[BBBMonsterGroundBlock] Actor=%s Component=%s Position=%s Normal=%s Delta=%s Penetrating=%d"),
                *GetPathNameSafe(Hit.GetActor()), *GetPathNameSafe(Hit.GetComponent()), *Start.ToString(),
                *Hit.Normal.ToString(), *Delta.ToString(), Hit.bStartPenetrating);
        }
        Location = bBlocked ? Start + Delta * Hit.Time : Start + Delta;
        bool bStepped = false;
        if (bBlocked && bWasGrounded && !HorizontalStep.IsNearlyZero() && Movement.MaxStepHeight > 0.0f)
        {
            FVector Candidate;
            FVector CandidateNormal;
            if (FBBBMonsterStep::Solve(World, Movement, Start, HorizontalStep, Params, Candidate, CandidateNormal))
            {
                Location = Candidate;
                Ground.SupportNormal = CandidateNormal;
                bStepped = true;
            }
        }

        if (bBlocked && !bStepped && !Hit.bStartPenetrating)
        {
            // 接触留缝不能超过已经向表面行进的距离 避免低速贴墙时被反复推回
            const float ApproachDistance = FMath::Max(0.0f, -FVector::DotProduct(Delta * Hit.Time, Hit.Normal));
            Location += Hit.Normal * FMath::Min(Skin, ApproachDistance);
            FVector Remainder = FVector::VectorPlaneProject(Delta * (1.0f - Hit.Time), Hit.Normal);
            if (!Walkable(Hit) || (bWasGrounded && Hit.ImpactPoint.Z > Start.Z - Movement.CapsuleHalfHeight + Movement.MaxStepHeight + Skin))
            {
                Remainder.Z = FMath::Min(Remainder.Z, 0.0);
            }
            FHitResult SlideHit;
            const bool bSlideBlocked = Sweep(Location, Location + Remainder, SlideHit);
            Location += Remainder * (bSlideBlocked ? SlideHit.Time : 1.0f);
            if (Walkable(Hit) && Velocity.Z <= 0.0f)
            {
                Velocity.Z = 0.0f;
            }
            if (Hit.Normal.Z < -KINDA_SMALL_NUMBER && Velocity.Z > 0.0f)
            {
                Velocity.Z = 0.0f;
            }
        }

        const float SnapDistance = bWasGrounded ? Movement.MaxStepHeight + SupportDistance : SupportDistance;
        Ground.bGrounded = Velocity.Z <= 0.0f && FindSupport(Location, SnapDistance, Ground.SupportNormal);
        if (Ground.bGrounded)
        {
            Velocity.Z = 0.0f;
        }
        if (!Ground.bGrounded)
        {
            Ground.SupportNormal = FVector::UpVector;
        }
    }
    Ground.MovedDistance = FVector::Dist2D(Location, FrameStart);
    Velocity.X = (Location.X - FrameStart.X) / SimulationSeconds;
    Velocity.Y = (Location.Y - FrameStart.Y) / SimulationSeconds;
}

void UBBBMonsterLocomotionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterTraversalFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterSpecialFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
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
        auto Grounds = Chunk.GetMutableFragmentView<FBBBMonsterGroundFragment>();
        const auto Traversals = Chunk.GetFragmentView<FBBBMonsterTraversalFragment>();
        auto Navigation = Chunk.GetMutableFragmentView<FBBBMonsterNavigationFragment>();
        const auto Avoidance = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Mobility = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            const auto Specials = Chunk.GetFragmentView<FBBBMonsterSpecialFragment>();
            if (Traversals[Index].Fact.Kind != 0 || (!Specials.IsEmpty() && Specials[Index].Fact.HoldsMovement()))
            {
                continue;
            }
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            FVector& Velocity = Velocities[Index].Value;
            FBBBMonsterMovementFragment& Movement = Movements[Index];
            FBBBMonsterNavigationFragment& Path = Navigation[Index];
            const bool bPatrol = States[Index].State == EBBBMonsterBehavior::Patrol;
            const bool bChase = States[Index].State == EBBBMonsterBehavior::Chase;
            if (DeltaSeconds <= SMALL_NUMBER)
            {
                continue;
            }
            FVector Location = Transform.GetLocation();
            const auto& Injury = Mobility[Index];
            const auto* Settings = Network[Index].Definition.Get();
            const float Now = World->GetTimeSeconds();
            const auto& Target = Targets[Index];
            const float TargetDistance = Target.bHasTarget ? FVector::Distance(Location, Target.TargetLocation) : MAX_flt;
            const bool bCrowdStationary = Velocity.Size2D() < 10.0f
                && Avoidance[Index].bCrowdConstrained;
            FBBBMonsterSteering::UpdateFacingMode(Movement, Target.bHasTarget,
                States[Index].State == EBBBMonsterBehavior::Alert || States[Index].State == EBBBMonsterBehavior::Attack,
                bChase, bCrowdStationary, TargetDistance, Settings ? Settings->AttackRange : 0.0f,
                Movement.CapsuleRadius, Now);
            if (!bChase || !Target.bHasTarget || Injury.bCrawling)
            {
                Movement.bWalkerRunning = false;
            }
            if (bChase && Target.bHasTarget && !Injury.bCrawling && Movement.MaxGait == EBBBMonsterGait::Walk &&
                FVector::DistSquared2D(Location, Target.TargetLocation) <= FMath::Square(Movement.WalkerRunDistance))
            {
                Movement.bWalkerRunning = true;
            }
            FBBBMonsterMovementFragment Geometry = Movement;
            bool bChangingPosture = false;
            if (Injury.bCrawling && Settings)
            {
                const float Progress = FMath::Clamp((Now - Injury.CrawlStartedAt) / Settings->CrawlTransitionDuration, 0.0f, 1.0f);
                Geometry.CapsuleHalfHeight = FMath::Lerp(Movement.CapsuleHalfHeight, Settings->CrawlCapsuleHalfHeight, Progress);
                Geometry.MaxStepHeight = Settings->CrawlMaxStepHeight;
                bChangingPosture = Progress < 1.0f;
            }
            if (Grounds[Index].CapsuleHalfHeight > 0.0f && Grounds[Index].bGrounded)
            {
                Location.Z += Geometry.CapsuleHalfHeight - Grounds[Index].CapsuleHalfHeight;
            }
            Grounds[Index].CapsuleHalfHeight = Geometry.CapsuleHalfHeight;
            FVector HorizontalDelta = FVector::ZeroVector;
            const bool bMayMove = !bChangingPosture && !Injury.IsStaggering(Now) && Now >= Injury.HitStopEndsAt
                && (bPatrol || bChase) && Path.bHasPath && !Path.bReachedDestination;
            if (!bMayMove)
            {
                Movement.Gait = EBBBMonsterGait::Walk;
            }
            if (bMayMove && Grounds[Index].bGrounded)
            {
                Movement.Gait = SelectGait(Movement, Path.RemainingDistance, bPatrol);
                const FVector DesiredFeet = Location - FVector::UpVector * Geometry.CapsuleHalfHeight
                    + Avoidance[Index].RecommendedVelocity * DeltaSeconds;
                const FVector AllowedFeet = FBBBMonsterNavigation::Constrain(Path, DesiredFeet);
                HorizontalDelta = FVector(AllowedFeet.X - Location.X, AllowedFeet.Y - Location.Y, 0);
                const float MaxDelta = Movement.SprintSpeed * DeltaSeconds + 1;
                HorizontalDelta = HorizontalDelta.GetClampedToMaxSize(MaxDelta);
                // 大角度改向先转身 小角度随朝向连续恢复行进
                const float Alignment = FMath::Max(0.0f, FVector::DotProduct(
                    Transform.GetRotation().GetForwardVector().GetSafeNormal2D(), HorizontalDelta.GetSafeNormal2D()));
                HorizontalDelta *= Alignment * Alignment;
            }
            if (bMayMove && !Grounds[Index].bGrounded)
            {
                HorizontalDelta = FVector(Velocity.X, Velocity.Y, 0.0f) * DeltaSeconds;
                if (Now < Injury.HitStopEndsAt)
                {
                    HorizontalDelta = FVector::ZeroVector;
                }
            }

            const bool bPreviouslyGrounded = Grounds[Index].bGrounded;
            SolveGroundMotion(*World, Geometry, Grounds[Index], Location, Velocity, HorizontalDelta, DeltaSeconds);
            if (!ensureMsgf(!Location.ContainsNaN() && !Velocity.ContainsNaN(), TEXT("[UBBBM]Ground solver produced invalid motion")))
            {
                continue;
            }
            Transform.SetLocation(Location);
            if (Grounds[Index].bGrounded)
            {
                FBBBMonsterNavigation::Feedback(Path, Location - FVector::UpVector * Geometry.CapsuleHalfHeight);
            }
            if (Grounds[Index].bGrounded)
            {
                Grounds[Index].UnsupportedSince = -1.0f;
            }
            if (!Grounds[Index].bGrounded && Grounds[Index].UnsupportedSince < 0.0f)
            {
                Grounds[Index].UnsupportedSince = Now;
            }
            if (bPreviouslyGrounded != Grounds[Index].bGrounded)
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Ground Entity=%d Supported=%d Z=%.2f VerticalSpeed=%.2f"),
                    Chunk.GetEntity(Index).Index, Grounds[Index].bGrounded, Location.Z, Velocity.Z);
            }
            FVector FacingDirection = bMayMove ? Avoidance[Index].RecommendedVelocity.GetSafeNormal2D() : FVector::ZeroVector;
            const bool bAtClimbEntry = Path.TraversalPoly != 0
                && FVector::DistSquared2D(Location, Path.TraversalEntry) <= FMath::Square(Movement.CapsuleRadius * 2);
            if (bAtClimbEntry)
            {
                FacingDirection = (Path.TraversalExit - Path.TraversalEntry).GetSafeNormal2D();
            }
            if (Movement.bFacingTarget && !bAtClimbEntry &&
                (!bMayMove || Avoidance[Index].RecommendedVelocity.SizeSquared2D() <= FMath::Square(10.0f)))
            {
                FacingDirection = (Target.TargetLocation - Location).GetSafeNormal2D();
            }
            if (States[Index].State != EBBBMonsterBehavior::Attack && States[Index].State != EBBBMonsterBehavior::Dead
                && !Injury.IsStaggering(Now) && Settings && !FacingDirection.IsNearlyZero())
            {
                const float Rate = Injury.bCrawling ? Settings->CrawlTurnRate
                    : Movement.Gait == EBBBMonsterGait::Walk ? Settings->WalkTurnRate : Settings->RunTurnRate;
                // 连续意图也负责起步前的转身 停止与碰撞修正本身不产生新方向
                Transform.SetRotation(Movement.bFacingTarget || Velocity.Size2D() <= 10.0f
                    ? TurnTowards(Transform.GetRotation(), FacingDirection, DeltaSeconds, Rate)
                    : CalculateFacing(Transform.GetRotation(), FacingDirection, Velocity, DeltaSeconds, Rate));
            }
        }
    });
}
