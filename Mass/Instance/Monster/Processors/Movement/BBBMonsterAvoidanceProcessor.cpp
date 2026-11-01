#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigationProcessor.h"
#include "BBBMonsterSteering.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"

UBBBMonsterAvoidanceProcessor::UBBBMonsterAvoidanceProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteAfter.Add(UBBBMonsterNavigationProcessor::StaticClass()->GetFName());
}

void UBBBMonsterAvoidanceProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNavigationFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
}

void UBBBMonsterAvoidanceProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 先确定空间分桶使用的最大搜索尺寸
    float CellSize = 1.0f;

    // 使用最大邻居范围作为空间分桶尺寸
    MonsterQuery.ForEachEntityChunk(Context, [&CellSize](FMassExecutionContext& ChunkContext)
    {
        const TConstArrayView<FBBBMonsterAvoidanceFragment> Avoidances = ChunkContext.GetFragmentView<FBBBMonsterAvoidanceFragment>();

        const auto Movements = ChunkContext.GetFragmentView<FBBBMonsterMovementFragment>();
        for (int32 Index = 0; Index < Avoidances.Num(); ++Index)
        {
            CellSize = FMath::Max(CellSize, FMath::Max(Avoidances[Index].NeighborSearchRadius,
                Avoidances[Index].CollisionRadius * 2.0f + Movements[Index].SprintSpeed * 0.2f));
        }
    });

    TMap<FIntPoint, TArray<TTuple<FMassEntityHandle, FVector, float>>> SpatialBuckets;

    // 第一遍遍历建立二维空间桶
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();

        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Avoidances = ChunkContext.GetFragmentView<FBBBMonsterAvoidanceFragment>();

        for (int32 Index = 0; Index < Transforms.Num(); ++Index)
        {
            if (States[Index].State == EBBBMonsterBehavior::Dead)
            {
                continue;
            }

            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const FIntPoint Cell(
                FMath::FloorToInt(Location.X / CellSize),
                FMath::FloorToInt(Location.Y / CellSize));
            SpatialBuckets.FindOrAdd(Cell).Emplace(ChunkContext.GetEntity(Index), Location, Avoidances[Index].CollisionRadius);
        }
    });

    // 第二遍遍历写入连续意图与安全推进范围
    const float Now = Context.GetWorld()->GetTimeSeconds();
    const float DeltaSeconds = FMath::Max(Context.GetDeltaTimeSeconds(), 0.0f);
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize, Now, DeltaSeconds](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        TArrayView<FBBBMonsterAvoidanceFragment> Avoidances = ChunkContext.GetMutableFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Paths = ChunkContext.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Movements = ChunkContext.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        const auto Networks = ChunkContext.GetFragmentView<FBBBMonsterNetworkFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            // 停止状态不再被分离修正推动 活体仍作为其它实体的障碍
            if (States[Index].State != EBBBMonsterBehavior::Chase && States[Index].State != EBBBMonsterBehavior::Patrol)
            {
                Avoidances[Index].SeparationDirection = FVector::ZeroVector;
                Avoidances[Index].SeparationStrength = 0.0f;
                Avoidances[Index].bWaitingForNeighbor = false;
                Avoidances[Index].SteeringSide = 0;
                Avoidances[Index].SideHeldUntil = 0.0f;
                Avoidances[Index].SteeringDirection = FVector::ZeroVector;
                Avoidances[Index].MaxTravelDistance = 0.0f;
                Avoidances[Index].bCrowdConstrained = false;
                continue;
            }

            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const FBBBMonsterAvoidanceFragment& Settings = Avoidances[Index];
            const float SearchRadius = FMath::Max(Settings.NeighborSearchRadius,
                Settings.CollisionRadius * 2.0f + Movements[Index].SprintSpeed * 0.2f);
            const float PersonalSpace = FMath::Clamp(Settings.PersonalSpaceRadius, 0.0f, SearchRadius);
            const FIntPoint CenterCell(
                FMath::FloorToInt(Location.X / CellSize),
                FMath::FloorToInt(Location.Y / CellSize));
            FVector Separation = FVector::ZeroVector;
            const auto& Path = Paths[Index];
            const FVector Goal = Path.bHasFollowTarget ? Path.FollowTarget
                : Path.PathPoints.IsValidIndex(Path.PathPointIndex) ? Path.PathPoints[Path.PathPointIndex] : Location;
            const FVector DesiredDirection = (Goal - Location).GetSafeNormal2D();
            constexpr int32 CandidateCount = 8;
            const float Side = (ChunkContext.GetEntity(Index).Index & 1) == 0 ? 1.0f : -1.0f;
            const float Angles[CandidateCount] = {0.0f, 25.0f, -25.0f, 50.0f, -50.0f, 75.0f, -75.0f, 0.0f};
            FVector Directions[CandidateCount];
            FVector Intents[CandidateCount];
            float Clearances[CandidateCount];
            float OpenDistances[CandidateCount];
            int8 Sides[CandidateCount];
            const auto* Definition = Networks[Index].Definition.Get();
            const float TurnRate = Definition ? (Movements[Index].Gait == EBBBMonsterGait::Walk
                ? Definition->WalkTurnRate : Definition->RunTurnRate) : 260.0f;
            const float Horizon = FMath::Max(1.0f, FMath::Min(SearchRadius - Settings.CollisionRadius * 2.0f,
                Movements[Index].SprintSpeed * 0.2f));
            const float Travel = FMath::Max(1.0f, Velocities[Index].Value.Size2D() * DeltaSeconds
                + Movements[Index].Acceleration * DeltaSeconds * DeltaSeconds);
            for (int32 Candidate = 0; Candidate < CandidateCount; ++Candidate)
            {
                FVector Intent = DesiredDirection.RotateAngleAxis(Angles[Candidate] * Side, FVector::UpVector);
                if (Candidate == CandidateCount - 1)
                {
                    Intent = Settings.SteeringDirection;
                }
                Intents[Candidate] = Intent;
                Directions[Candidate] = FBBBMonsterSteering::ConstrainDirection(Velocities[Index].Value,
                    Intent, Movements[Index].Acceleration, TurnRate, DeltaSeconds);
                const float Cross = FVector::CrossProduct(DesiredDirection, Intent).Z;
                Sides[Candidate] = FMath::Abs(Cross) < 0.05f ? 0 : Cross > 0.0f ? 1 : -1;
                Clearances[Candidate] = FVector::DotProduct(DesiredDirection, Directions[Candidate]) > 0.05f ? Horizon : 0.0f;
                OpenDistances[Candidate] = Intent.IsNearlyZero() ? 0.0f : Horizon;
            }

            // 只检查当前实体周边的相邻空间桶
            for (int32 OffsetX = -1; OffsetX <= 1; ++OffsetX)
            {
                for (int32 OffsetY = -1; OffsetY <= 1; ++OffsetY)
                {
                    const auto* Neighbors = SpatialBuckets.Find(CenterCell + FIntPoint(OffsetX, OffsetY));

                    if (Neighbors == nullptr)
                    {
                        continue;
                    }

                    for (const auto& Neighbor : *Neighbors)
                    {
                        const auto NeighborEntity = Neighbor.Get<0>();
                        const FVector& NeighborLocation = Neighbor.Get<1>();
                        if (NeighborEntity == ChunkContext.GetEntity(Index))
                        {
                            continue;
                        }
                        /** 二维分桶只负责候选检索 不把上下楼层当成身体重叠 */
                        if (FMath::Abs(Location.Z - NeighborLocation.Z) > Settings.CollisionRadius * 2.0f)
                        {
                            continue;
                        }
                        FVector AwayFromNeighbor = Location - NeighborLocation;
                        AwayFromNeighbor.Z = 0.0f;
                        if (AwayFromNeighbor.IsNearlyZero())
                        {
                            AwayFromNeighbor.X = ChunkContext.GetEntity(Index).Index < NeighborEntity.Index ? -0.01f : 0.01f;
                        }
                        const double DistanceSquared = AwayFromNeighbor.SizeSquared();

                        if (DistanceSquared >= FMath::Square(SearchRadius))
                        {
                            continue;
                        }
                        const float BodyDistance = Settings.CollisionRadius + Neighbor.Get<2>();
                        const double Distance = FMath::Sqrt(FMath::Max(DistanceSquared, 0.0001));
                        for (int32 Candidate = 0; Candidate < CandidateCount; ++Candidate)
                        {
                            // 长程净空只负责选绕行侧 本帧连续方向仍遵守双方间距约束
                            const float IntentAlong = FVector::DotProduct(-AwayFromNeighbor, Intents[Candidate]);
                            const double LateralSquared = FMath::Max(0.0, DistanceSquared - FMath::Square(IntentAlong));
                            if (IntentAlong > 0.0f && LateralSquared < FMath::Square(BodyDistance))
                            {
                                OpenDistances[Candidate] = FMath::Min(OpenDistances[Candidate],
                                    FMath::Max(0.0f, IntentAlong - static_cast<float>(FMath::Sqrt(FMath::Square(BodyDistance) - LateralSquared))));
                            }
                            const float Along = FVector::DotProduct(-AwayFromNeighbor, Directions[Candidate]);
                            if (DistanceSquared < FMath::Square(BodyDistance))
                            {
                                if (Along >= 0.0f)
                                {
                                    Clearances[Candidate] = 0.0f;
                                }
                                continue;
                            }
                            if (Along <= 0.0f)
                            {
                                continue;
                            }
                            /** 两侧各承担一半法向间距 保留切向通行 避免交叉移动时同时侵入 */
                            const float NormalAllowance = FMath::Max(0.0, Distance - BodyDistance - 1.0) * 0.5;
                            Clearances[Candidate] = FMath::Min(Clearances[Candidate],
                                static_cast<float>(NormalAllowance * Distance / Along));
                        }
                        if (Distance < PersonalSpace)
                        {
                            Separation += AwayFromNeighbor / Distance * (1.0f - Distance / FMath::Max(PersonalSpace, 1.0f));
                        }
                    }
                }
            }

            Avoidances[Index].SeparationStrength = Separation.Size();
            Avoidances[Index].SeparationDirection = Separation.GetSafeNormal();
            auto& Result = Avoidances[Index];
            int32 Best = 0;
            int32 Preferred = INDEX_NONE;
            float BestScore = -FLT_MAX;
            float PreferredScore = -FLT_MAX;
            for (int32 Candidate = 0; Candidate < CandidateCount; ++Candidate)
            {
                // 前方净空选择绕行意图 本帧许可距离约束真实移动 所有候选均保持前进半平面
                const float Progress = FMath::Clamp(Clearances[Candidate] / Travel, 0.0f, 1.0f);
                const float Forward = FVector::DotProduct(Intents[Candidate], DesiredDirection);
                const float Score = Progress * Forward * 0.25f + OpenDistances[Candidate] / Horizon * 0.75f - (1.0f - Forward) * 0.15f
                    + FVector::DotProduct(Directions[Candidate], Result.SeparationDirection)
                        * FMath::Min(Result.SeparationStrength * Result.AvoidanceWeight, 1.0f) * 0.05f
                    + FVector::DotProduct(Directions[Candidate], Result.SteeringDirection) * 0.1f;
                if (Score > BestScore)
                {
                    Best = Candidate;
                    BestScore = Score;
                }
                if ((Sides[Candidate] == 0 || Sides[Candidate] == Result.SteeringSide)
                    && Score > PreferredScore && Clearances[Candidate] > 0.25f)
                {
                    Preferred = Candidate;
                    PreferredScore = Score;
                }
            }
            if (Result.SteeringSide != 0 && Preferred != INDEX_NONE
                && (Now < Result.SideHeldUntil || BestScore < PreferredScore + 0.15f))
            {
                Best = Preferred;
            }
            const int8 ChosenSide = Sides[Best];
            if (ChosenSide != 0 && ChosenSide != Result.SteeringSide && Clearances[Best] > 0.25f)
            {
                Result.SideChanges += Result.SteeringSide != 0 ? 1u : 0u;
                Result.SteeringSide = ChosenSide;
                Result.SideHeldUntil = Now + Result.SideHoldDuration;
            }
            Result.SteeringDirection = Directions[Best];
            Result.MaxTravelDistance = Clearances[Best];
            Result.bCrowdConstrained = Clearances[0] < Horizon || Clearances[Best] < Horizon;
            Result.bWaitingForNeighbor = Result.bCrowdConstrained && Result.MaxTravelDistance <= 0.25f;
        }
    });
}
