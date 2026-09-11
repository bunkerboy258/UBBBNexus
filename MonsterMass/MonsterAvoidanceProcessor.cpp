#include "BBBWork/UBBBNexus/MonsterMass/MonsterAvoidanceProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterNavigationProcessor.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

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
    MonsterQuery.AddTagRequirement<FMonsterTag>(EMassFragmentPresence::All);
}

void UMonsterAvoidanceProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    float CellSize = 1.0f;

    MonsterQuery.ForEachEntityChunk(Context, [&CellSize](FMassExecutionContext& ChunkContext)
    {
        const TConstArrayView<FMonsterAvoidanceFragment> Avoidances = ChunkContext.GetFragmentView<FMonsterAvoidanceFragment>();

        for (const FMonsterAvoidanceFragment& Avoidance : Avoidances)
        {
            CellSize = FMath::Max(CellSize, Avoidance.NeighborSearchRadius);
        }
    });

    TMap<FIntPoint, TArray<FVector>> SpatialBuckets;

    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();

        for (const FTransformFragment& Transform : Transforms)
        {
            const FVector Location = Transform.GetTransform().GetLocation();
            const FIntPoint Cell(
                FMath::FloorToInt(Location.X / CellSize),
                FMath::FloorToInt(Location.Y / CellSize));
            SpatialBuckets.FindOrAdd(Cell).Add(Location);
        }
    });

    MonsterQuery.ForEachEntityChunk(Context, [&SpatialBuckets, CellSize](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        TArrayView<FMonsterAvoidanceFragment> Avoidances = ChunkContext.GetMutableFragmentView<FMonsterAvoidanceFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            const FVector Location = Transforms[Index].GetTransform().GetLocation();
            const FMonsterAvoidanceFragment& Settings = Avoidances[Index];
            const float SearchRadius = FMath::Max(Settings.NeighborSearchRadius, 1.0f);
            const float PersonalSpace = FMath::Clamp(Settings.PersonalSpaceRadius, 0.0f, SearchRadius);
            const FIntPoint CenterCell(
                FMath::FloorToInt(Location.X / CellSize),
                FMath::FloorToInt(Location.Y / CellSize));
            FVector Separation = FVector::ZeroVector;
            float ClosestNeighborDistance = PersonalSpace;

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

            const float CorrectionDistance = (PersonalSpace - ClosestNeighborDistance) * 0.5f;
            FTransform& Transform = Transforms[Index].GetMutableTransform();
            Transform.SetLocation(Location + Avoidances[Index].SeparationDirection * CorrectionDistance);
        }
    });
}
