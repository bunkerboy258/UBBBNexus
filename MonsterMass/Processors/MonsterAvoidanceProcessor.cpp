#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterAvoidanceProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/Processors/MonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

UMonsterAvoidanceProcessor::UMonsterAvoidanceProcessor()
    : MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UMonsterNavigationProcessor::StaticClass()->GetFName());
}

void UMonsterAvoidanceProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterAvoidanceFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddRequirement<FMonsterStateFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterAvoidanceProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // 先确定空间分桶使用的最大搜索尺寸
    float CellSize = 1.0f;

    // 使用最大邻居范围作为空间分桶尺寸
    MonsterQuery.ForEachEntityChunk(Context, [&CellSize](FMassExecutionContext& ChunkContext)
    {
        const TConstArrayView<FMonsterAvoidanceFragment> Avoidances = ChunkContext.GetFragmentView<FMonsterAvoidanceFragment>();

        for (const FMonsterAvoidanceFragment& Avoidance : Avoidances)
        {
            CellSize = FMath::Max(CellSize, Avoidance.NeighborSearchRadius);
        }
    });

    TMap<FIntPoint, TArray<FVector>> SpatialBuckets;

    // 第一遍遍历建立二维空间桶
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();

        const auto States = ChunkContext.GetFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < Transforms.Num(); ++Index)
        {
            if (States[Index].State == EMonsterState::Dead)
            {
                continue;
            }

            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const FIntPoint Cell(
                FMath::FloorToInt(Location.X / CellSize),
                FMath::FloorToInt(Location.Y / CellSize));
            SpatialBuckets.FindOrAdd(Cell).Add(Location);
        }
    });

    // 第二遍遍历计算分离方向并修正位置
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        TArrayView<FMonsterAvoidanceFragment> Avoidances = ChunkContext.GetMutableFragmentView<FMonsterAvoidanceFragment>();
        const auto States = ChunkContext.GetFragmentView<FMonsterStateFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            // 停止状态不再被分离修正推动 活体仍作为其它实体的障碍
            if (States[Index].State != EMonsterState::Chase)
            {
                Avoidances[Index].SeparationDirection = FVector::ZeroVector;
                Avoidances[Index].SeparationStrength = 0.0f;
                continue;
            }

            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const FMonsterAvoidanceFragment& Settings = Avoidances[Index];
            const float SearchRadius = FMath::Max(Settings.NeighborSearchRadius, 1.0f);
            const float PersonalSpace = FMath::Clamp(Settings.PersonalSpaceRadius, 0.0f, SearchRadius);
            const FIntPoint CenterCell(
                FMath::FloorToInt(Location.X / CellSize),
                FMath::FloorToInt(Location.Y / CellSize));
            FVector Separation = FVector::ZeroVector;
            float ClosestNeighborDistance = PersonalSpace;

            // 只检查当前实体周边的相邻空间桶
            for (int32 OffsetX = -1; OffsetX <= 1; ++OffsetX)
            {
                for (int32 OffsetY = -1; OffsetY <= 1; ++OffsetY)
                {
                    const TArray<FVector>* Neighbors = SpatialBuckets.Find(CenterCell + FIntPoint(OffsetX, OffsetY));

                    if (Neighbors == nullptr)
                    {
                        continue;
                    }

                    for (const FVector& NeighborLocation : *Neighbors)
                    {
                        FVector AwayFromNeighbor = Location - NeighborLocation;
                        AwayFromNeighbor.Z = 0.0f;
                        const float Distance = AwayFromNeighbor.Size();

                        if (Distance <= KINDA_SMALL_NUMBER || Distance >= PersonalSpace)
                        {
                            continue;
                        }

                        ClosestNeighborDistance = FMath::Min(ClosestNeighborDistance, Distance);
                        Separation += AwayFromNeighbor / Distance * (1.0f - Distance / PersonalSpace);
                    }
                }
            }

            Avoidances[Index].SeparationStrength = Separation.Size();
            Avoidances[Index].SeparationDirection = Separation.GetSafeNormal();

            if (Avoidances[Index].SeparationDirection.IsNearlyZero())
            {
                continue;
            }

            // 按分离方向将过近实体推开
            const float CorrectionDistance = (PersonalSpace - ClosestNeighborDistance) * 0.5f;
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            Transform.SetLocation(Location + Avoidances[Index].SeparationDirection * CorrectionDistance);
        }
    });
}
