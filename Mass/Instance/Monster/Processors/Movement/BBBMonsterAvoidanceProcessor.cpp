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

    TMap<FIntPoint, TArray<TTuple<FMassEntityHandle, FVector, FVector, float>>> SpatialBuckets;

    // 第一遍遍历建立二维空间桶
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();

        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Paths = ChunkContext.GetFragmentView<FBBBMonsterNavigationFragment>();
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
            const auto& Path = Paths[Index];
            const FVector Goal = Path.bHasFollowTarget ? Path.FollowTarget
                : Path.PathPoints.IsValidIndex(Path.PathPointIndex) ? Path.PathPoints[Path.PathPointIndex] : Location;
            SpatialBuckets.FindOrAdd(Cell).Emplace(ChunkContext.GetEntity(Index), Location,
                States[Index].State == EBBBMonsterBehavior::Attack ? FVector::ZeroVector : (Goal - Location).GetSafeNormal2D(),
                Avoidances[Index].CollisionRadius);
        }
    });

    // 第二遍遍历只写入分离意图和有限让行 不直接修改位置
    const float Now = Context.GetWorld()->GetTimeSeconds();
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize, Now](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        TArrayView<FBBBMonsterAvoidanceFragment> Avoidances = ChunkContext.GetMutableFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Paths = ChunkContext.GetFragmentView<FBBBMonsterNavigationFragment>();
        const auto Movements = ChunkContext.GetFragmentView<FBBBMonsterMovementFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            // 停止状态不再被分离修正推动 活体仍作为其它实体的障碍
            if (States[Index].State != EBBBMonsterBehavior::Chase && States[Index].State != EBBBMonsterBehavior::Patrol)
            {
                Avoidances[Index].SeparationDirection = FVector::ZeroVector;
                Avoidances[Index].SeparationStrength = 0.0f;
                Avoidances[Index].bWaitingForNeighbor = false;
                Avoidances[Index].YieldBlocker.Reset();
                Avoidances[Index].YieldStartedAt = -1.0f;
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
            constexpr int32 CandidateCount = 9;
            const float Side = (ChunkContext.GetEntity(Index).Index & 1) == 0 ? 1.0f : -1.0f;
            const float Angles[CandidateCount] = {0.0f, 35.0f, -35.0f, 70.0f, -70.0f, 110.0f, -110.0f, 150.0f, -150.0f};
            FVector Directions[CandidateCount];
            float Clearances[CandidateCount];
            const float Horizon = FMath::Max(1.0f, FMath::Min(SearchRadius - Settings.CollisionRadius * 2.0f,
                Movements[Index].SprintSpeed * 0.2f));
            for (int32 Candidate = 0; Candidate < CandidateCount; ++Candidate)
            {
                Directions[Candidate] = DesiredDirection.RotateAngleAxis(Angles[Candidate] * Side, FVector::UpVector);
                Clearances[Candidate] = Horizon;
            }
            FMassEntityHandle Blocker;
            double BlockerDistance = MAX_dbl;

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
                        const FVector& NeighborDirection = Neighbor.Get<2>();
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
                        const float BodyDistance = Settings.CollisionRadius + Neighbor.Get<3>();
                        const double Distance = FMath::Sqrt(FMath::Max(DistanceSquared, 0.0001));
                        for (int32 Candidate = 0; Candidate < CandidateCount; ++Candidate)
                        {
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
                        const float ForwardDistance = FVector::DotProduct(-AwayFromNeighbor, DesiredDirection);
                        const double LateralSquared = FMath::Max(0.0, DistanceSquared - FMath::Square(ForwardDistance));
                        const bool bYieldPriority = NeighborDirection.IsNearlyZero()
                            || FVector::DotProduct(DesiredDirection, NeighborDirection) > 0.5f
                            || NeighborEntity.Index < ChunkContext.GetEntity(Index).Index;
                        if (bYieldPriority && ForwardDistance > 0.0f && Distance < Settings.CollisionRadius * 2.1f
                            && LateralSquared < FMath::Square(Settings.CollisionRadius * 1.2f) && DistanceSquared < BlockerDistance)
                        {
                            Blocker = NeighborEntity;
                            BlockerDistance = DistanceSquared;
                        }
                    }
                }
            }

            Avoidances[Index].SeparationStrength = Separation.Size();
            Avoidances[Index].SeparationDirection = Separation.GetSafeNormal();
            auto& Result = Avoidances[Index];
            int32 Best = 0;
            float BestScore = -FLT_MAX;
            for (int32 Candidate = 0; Candidate < CandidateCount; ++Candidate)
            {
                const float Score = (Clearances[Candidate] >= 1.0f ? 1.0f : 0.0f)
                    + Clearances[Candidate] / Horizon - FMath::Abs(Angles[Candidate]) / 180.0f
                    + FVector::DotProduct(Directions[Candidate], Result.SeparationDirection)
                        * FMath::Min(Result.SeparationStrength * Result.AvoidanceWeight, 1.0f) * 0.15f
                    + FVector::DotProduct(Directions[Candidate], Result.SteeringDirection) * 0.1f;
                if (Score > BestScore)
                {
                    Best = Candidate;
                    BestScore = Score;
                }
            }
            Result.SteeringDirection = Directions[Best];
            Result.MaxTravelDistance = Clearances[Best];
            Result.bCrowdConstrained = Clearances[0] < Horizon || Clearances[Best] < Horizon;
            if (!Blocker.IsSet())
            {
                Result.YieldStartedAt = -1.0f;
            }
            if (Blocker.IsSet() && Result.YieldStartedAt < 0.0f)
            {
                Result.YieldStartedAt = Now;
            }
            Result.YieldBlocker = Blocker;
            Result.bWaitingForNeighbor = Blocker.IsSet() && Result.YieldStartedAt >= 0.0f
                && Now - Result.YieldStartedAt < Result.CrowdWaitDuration && Result.MaxTravelDistance < 1.0f;
        }
    });
}
