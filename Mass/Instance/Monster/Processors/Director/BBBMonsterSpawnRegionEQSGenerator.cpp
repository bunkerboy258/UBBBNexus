#include "BBBMonsterSpawnRegionEQSGenerator.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterDirectorPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"

UBBBMonsterSpawnRegionEQSGenerator::UBBBMonsterSpawnRegionEQSGenerator(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ItemType = UEnvQueryItemType_Point::StaticClass();
    OptionName = TEXT("副本小怪区域候选点");
}

void UBBBMonsterSpawnRegionEQSGenerator::GenerateCandidates(const TArray<FBBBMonsterSpawnRegionDefinition>& Regions,
    const int32 Count, const int32 Seed, TArray<FVector>& Result)
{
    Result.Reset();
    if (Regions.IsEmpty() || Count <= 0)
    {
        return;
    }
    FRandomStream Random(Seed);
    for (int32 Index = 0; Index < FMath::Min(Count, 512); ++Index)
    {
        const auto& Region = Regions[Index % Regions.Num()];
        if (!Region.IsValid())
        {
            continue;
        }
        FVector Local;
        if (Region.Shape == EBBBMonsterSpawnRegionShape::Cylinder)
        {
            const float Angle = Random.FRand() * UE_TWO_PI;
            const float Radius = FMath::Sqrt(Random.FRand()) * Region.Extent.X;
            Local = FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0);
        }
        if (Region.Shape == EBBBMonsterSpawnRegionShape::Box)
        {
            Local = FVector(Random.FRandRange(-Region.Extent.X, Region.Extent.X), Random.FRandRange(-Region.Extent.Y, Region.Extent.Y), 0.0);
        }
        Result.Add(Region.Transform.TransformPosition(Local));
    }
}

void UBBBMonsterSpawnRegionEQSGenerator::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
    const auto* Owner = Cast<ABBBMonsterDirectorPresentationActor>(QueryInstance.Owner.Get());
    if (!Owner || !Owner->Definition)
    {
        return;
    }
    const int32 StageIndex = static_cast<int32>(QueryInstance.NamedParams.FindRef(TEXT("DirectorStage")));
    const int32 Seed = static_cast<int32>(QueryInstance.NamedParams.FindRef(TEXT("DirectorSeed")));
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;
    TArray<FVector> Candidates;
    if (Owner->ReadPlacementRegions(StageIndex, Regions))
    {
        GenerateCandidates(Regions, Owner->Definition->CandidateCount, Seed, Candidates);
        QueryInstance.AddItemData<UEnvQueryItemType_Point>(Candidates);
    }
}
