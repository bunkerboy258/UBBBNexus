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
#include "NavMesh/RecastNavMesh.h"
#include "Engine/World.h"
#include "CollisionShape.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterTargetFragment.h"

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
    if (bPatrol || Movement.MaxGait == EBBBMonsterGait::Walk)
    {
        return EBBBMonsterGait::Walk;
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
    for (int32 Attempt = 0; Attempt < 4; ++Attempt)
    {
        FHitResult Penetration;
        if (!Sweep(Location, Location, Penetration) || !Penetration.bStartPenetrating)
        {
            break;
        }
        Location += Penetration.Normal * (Penetration.PenetrationDepth + Skin);
    }

    // 静止活体仍逐帧检测真实支撑 无水平运动时不重复执行移动扫掠和落地吸附
    if (Ground.bGrounded && Velocity.Z <= 0.0f && HorizontalDelta.IsNearlyZero() &&
        FindSupport(Location, SupportDistance, Ground.SupportNormal))
    {
        Velocity = FVector::ZeroVector;
        return;
    }

    const int32 Steps = FMath::Max(1, FMath::CeilToInt(DeltaSeconds / (1.0f / 60.0f)));
    const float StepSeconds = DeltaSeconds / Steps;
    const FVector HorizontalStep = FVector(HorizontalDelta.X, HorizontalDelta.Y, 0.0f) / Steps;
    const FVector FrameStart = Location;
    for (int32 Step = 0; Step < Steps; ++Step)
    {
        Ground.bGrounded = Velocity.Z <= 0.0f && FindSupport(Location, SupportDistance, Ground.SupportNormal);
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
            const FVector Raised = Start + FVector::UpVector * Movement.MaxStepHeight;
            FVector Candidate = Raised + HorizontalStep;
            FVector CandidateNormal = FVector::UpVector;
            float CandidateSurfaceHeight = 0.0f;
            FHitResult UpHit;
            FHitResult ForwardHit;
            if (!Sweep(Start, Raised, UpHit) && !Sweep(Raised, Candidate, ForwardHit) &&
                FindSupport(Candidate, Movement.MaxStepHeight + SupportDistance, CandidateNormal, &CandidateSurfaceHeight) &&
                Candidate.Z <= Start.Z + Movement.MaxStepHeight + Skin &&
                CandidateSurfaceHeight <= Start.Z - Movement.CapsuleHalfHeight + Movement.MaxStepHeight + Skin)
            {
                Location = Candidate;
                Ground.SupportNormal = CandidateNormal;
                bStepped = true;
            }
        }
        if (bBlocked && !bStepped && !Hit.bStartPenetrating)
        {
            Location += Hit.Normal * Skin;
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
    Velocity.X = (Location.X - FrameStart.X) / DeltaSeconds;
    Velocity.Y = (Location.Y - FrameStart.Y) / DeltaSeconds;
}

void UBBBMonsterLocomotionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterTargetFragment>(EMassFragmentAccess::ReadOnly);
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
        const auto Navigation = Chunk.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Avoidance = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto States = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Mobility = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Targets = Chunk.GetFragmentView<FBBBMonsterTargetFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            FVector& Velocity = Velocities[Index].Value;
            FBBBMonsterMovementFragment& Movement = Movements[Index];
            const FBBBMonsterNavigationFragment& Path = Navigation[Index];
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
            const bool bMayMove = !bChangingPosture && (bPatrol || bChase) && Path.bHasPath &&
                Path.PathPoints.IsValidIndex(Path.PathPointIndex) && Path.TailDistances.IsValidIndex(Path.PathPointIndex);
            if (!bMayMove)
            {
                Movement.Gait = EBBBMonsterGait::Walk;
            }
            if (bMayMove && Grounds[Index].bGrounded)
            {
                const FVector ToPoint = Path.PathPoints[Path.PathPointIndex] - Location;
                const float PointDistance = ToPoint.Size2D();
                const float Remaining = FMath::Max(PointDistance + Path.TailDistances[Path.PathPointIndex] - (bPatrol ? 15.0f : 0.0f), 0.0f);
                Movement.Gait = SelectGait(Movement, Remaining, bPatrol);
                const float UprightSpeed = Movement.Gait == EBBBMonsterGait::Walk ? Movement.WalkSpeed :
                    Movement.Gait == EBBBMonsterGait::Run ? Movement.RunSpeed : Movement.SprintSpeed;
                const float SpeedRatio = Injury.GetSpeedRatio(Now);
                const float TargetSpeed = (Injury.bCrawling && Settings ? Settings->CrawlSpeed : UprightSpeed) * SpeedRatio;
                const float InterpolatedSpeed = CalculateSpeed(Movement, Velocity.Size2D(), TargetSpeed, Remaining, DeltaSeconds);
                // 受击压制在保持阶段直接限制实际速度 恢复阶段仍由正常加速度推进
                const float Speed = Now < Injury.SlowRecoveryStartedAt ? FMath::Min(InterpolatedSpeed, TargetSpeed) : InterpolatedSpeed;
                const float Distance = FMath::Min3(Speed * DeltaSeconds, PointDistance, Remaining);
                const FBBBMonsterAvoidanceFragment& Neighbors = Avoidance[Index];
                const float Steering = FMath::Clamp(Neighbors.SeparationStrength * Neighbors.AvoidanceWeight, 0.0f, 0.65f);
                const FVector Direction = (ToPoint.GetSafeNormal2D() + Neighbors.SeparationDirection * Steering).GetSafeNormal2D();
                FVector Destination = Location + Direction * Distance;

                // 同一求解内约束导航边界 防止避让把实体推到墙外
                const FVector FootOffset(0.0f, 0.0f, Geometry.CapsuleHalfHeight);
                auto* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
                const auto* NavMesh = NavigationSystem ? Cast<ARecastNavMesh>(NavigationSystem->GetDefaultNavDataInstance()) : nullptr;
                FNavLocation ProjectedFoot;
                if (!NavMesh || !NavigationSystem->ProjectPointToNavigation(Location - FootOffset, ProjectedFoot,
                    FVector(Geometry.CapsuleRadius, Geometry.CapsuleRadius, 250.0f), NavMesh))
                {
                    Destination = Location;
                }
                else
                {
                    // 使用最近导航多边形作为唯一射线起点 允许边界外的浅层偏移沿物理可行方向进入导航区
                    const FVector NavEnd = ProjectedFoot.Location + Direction * Distance;
                    FVector HitLocation;
                    if (ARecastNavMesh::NavMeshRaycast(NavMesh, ProjectedFoot.NodeRef, ProjectedFoot.Location,
                        NavEnd, HitLocation, NavMesh->GetDefaultQueryFilter()))
                    {
                        const float Travel = FMath::Clamp(FVector::DotProduct(HitLocation - ProjectedFoot.Location, Direction), 0.0, double(Distance));
                        Destination = Location + Direction * Travel;
                    }
                }
                if (!Destination.ContainsNaN() && FVector::Dist2D(Location, Destination) <= Distance + 1.0f)
                {
                    HorizontalDelta = FVector(Destination.X - Location.X, Destination.Y - Location.Y, 0.0f);
                }
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
            if (bPreviouslyGrounded != Grounds[Index].bGrounded)
            {
                UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Ground Entity=%d Supported=%d Z=%.2f VerticalSpeed=%.2f"),
                    Chunk.GetEntity(Index).Index, Grounds[Index].bGrounded, Location.Z, Velocity.Z);
            }
            FVector FacingDirection = HorizontalDelta;
            const auto& Target = Targets[Index];
            const bool bFaceTarget = Target.bHasTarget && Settings && (States[Index].State == EBBBMonsterBehavior::Alert
                || (bChase && FVector::DistSquared(Location, Target.TargetLocation) <= FMath::Square(Settings->AttackRange)));
            if (bFaceTarget)
            {
                FacingDirection = (Target.TargetLocation - Location).GetSafeNormal2D();
            }
            if (States[Index].State != EBBBMonsterBehavior::Attack && States[Index].State != EBBBMonsterBehavior::Dead
                && !Injury.IsStaggering(Now) && Settings && !FacingDirection.IsNearlyZero())
            {
                const float Rate = Injury.bCrawling ? Settings->CrawlTurnRate
                    : Movement.Gait == EBBBMonsterGait::Walk ? Settings->WalkTurnRate : Settings->RunTurnRate;
                Transform.SetRotation(bFaceTarget ? TurnTowards(Transform.GetRotation(), FacingDirection, DeltaSeconds, Rate)
                    : CalculateFacing(Transform.GetRotation(), HorizontalDelta, Velocity, DeltaSeconds, Rate));
            }
        }
    });
}
