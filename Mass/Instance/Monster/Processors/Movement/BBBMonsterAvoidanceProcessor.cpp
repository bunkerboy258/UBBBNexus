#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
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

        for (const FBBBMonsterAvoidanceFragment& Avoidance : Avoidances)
        {
            CellSize = FMath::Max(CellSize, Avoidance.NeighborSearchRadius);
        }
    });

    TMap<FIntPoint, TArray<TTuple<FMassEntityHandle, FVector, FVector>>> SpatialBuckets;

    // 第一遍遍历建立二维空间桶
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();

        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();
        const auto Paths = ChunkContext.GetFragmentView<FBBBMonsterNavigationFragment>();

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
            SpatialBuckets.FindOrAdd(Cell).Emplace(ChunkContext.GetEntity(Index), Location, (Goal - Location).GetSafeNormal2D());
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
                continue;
            }

            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const FBBBMonsterAvoidanceFragment& Settings = Avoidances[Index];
            const float SearchRadius = FMath::Max(Settings.NeighborSearchRadius, 1.0f);
            const float PersonalSpace = FMath::Clamp(Settings.PersonalSpaceRadius, 0.0f, SearchRadius);
            const FIntPoint CenterCell(
                FMath::FloorToInt(Location.X / CellSize),
                FMath::FloorToInt(Location.Y / CellSize));
            FVector Separation = FVector::ZeroVector;
            const auto& Path = Paths[Index];
            const FVector Goal = Path.bHasFollowTarget ? Path.FollowTarget
                : Path.PathPoints.IsValidIndex(Path.PathPointIndex) ? Path.PathPoints[Path.PathPointIndex] : Location;
            const FVector DesiredDirection = (Goal - Location).GetSafeNormal2D();
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
                        /** 二维分桶只负责候选检索 不把上下楼层当成身体重叠 */
                        if (FMath::Abs(Location.Z - NeighborLocation.Z) > Settings.CollisionRadius * 2.0f)
                        {
                            continue;
                        }
                        FVector AwayFromNeighbor = Location - NeighborLocation;
                        AwayFromNeighbor.Z = 0.0f;
                        const double DistanceSquared = AwayFromNeighbor.SizeSquared();

                        if (DistanceSquared <= FMath::Square(KINDA_SMALL_NUMBER) || DistanceSquared >= FMath::Square(PersonalSpace))
                        {
                            continue;
                        }

                        const double Distance = FMath::Sqrt(DistanceSquared);
                        Separation += AwayFromNeighbor / Distance * (1.0f - Distance / PersonalSpace);
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
            if (Blocker != Result.YieldBlocker)
            {
                Result.YieldBlocker = Blocker;
                Result.YieldStartedAt = Blocker.IsSet() ? Now : -1.0f;
            }
            Result.bWaitingForNeighbor = Blocker.IsSet() && Result.YieldStartedAt >= 0.0f
                && Now - Result.YieldStartedAt < Result.CrowdWaitDuration;
        }
    });
}
