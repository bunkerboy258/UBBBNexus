#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
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

    TMap<FIntPoint, TArray<FVector>> SpatialBuckets;

    // 第一遍遍历建立二维空间桶
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();

        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();

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
            SpatialBuckets.FindOrAdd(Cell).Add(Location);
        }
    });

    // 第二遍遍历计算分离方向并修正位置
    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        const auto Transforms = ChunkContext.GetFragmentView<FTransformFragment>();
        TArrayView<FBBBMonsterAvoidanceFragment> Avoidances = ChunkContext.GetMutableFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto States = ChunkContext.GetFragmentView<FBBBMonsterBehaviorFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            // 停止状态不再被分离修正推动 活体仍作为其它实体的障碍
            if (States[Index].State != EBBBMonsterBehavior::Chase && States[Index].State != EBBBMonsterBehavior::Patrol)
            {
                Avoidances[Index].SeparationDirection = FVector::ZeroVector;
                Avoidances[Index].SeparationStrength = 0.0f;
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

                        Separation += AwayFromNeighbor / Distance * (1.0f - Distance / PersonalSpace);
                    }
                }
            }

            Avoidances[Index].SeparationStrength = Separation.Size();
            Avoidances[Index].SeparationDirection = Separation.GetSafeNormal();

        }
    });
}
