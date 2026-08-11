#include "BBBWork/UBBBNexus/MonsterMass/MonsterSpawnPointsGenerator.h"

#include "GameFramework/Actor.h"
#include "MassCommonUtils.h"
#include "MassSpawnLocationProcessor.h"
#include "MassSpawnerTypes.h"

void UMonsterSpawnPointsGenerator::Generate(
    UObject& QueryOwner,
    TConstArrayView<FMassSpawnedEntityType> EntityTypes,
    const int32 Count,
    FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const
{
    if (Count <= 0)
    {
        FinishedGeneratingSpawnPointsDelegate.Execute(TArray<FMassEntitySpawnDataGeneratorResult>());
        return;
    }

    const AActor* SpawnOwner = Cast<AActor>(&QueryOwner);

    if (!ensureMsgf(SpawnOwner != nullptr, TEXT("[UBBBM]Spawn point generator owner must be an actor")))
    {
        FinishedGeneratingSpawnPointsDelegate.Execute(TArray<FMassEntitySpawnDataGeneratorResult>());
        return;
    }

    TArray<FMassEntitySpawnDataGeneratorResult> Results;
    BuildResultsFromEntityTypes(Count, EntityTypes, Results);

    const FVector SpawnCenter = SpawnOwner->GetActorLocation();
    FRandomStream RandomStream(UE::Mass::Utils::OverrideRandomSeedForTesting(GetRandomSelectionSeed()));

    for (FMassEntitySpawnDataGeneratorResult& Result : Results)
    {
        Result.SpawnDataProcessor = UMassSpawnLocationProcessor::StaticClass();
        Result.SpawnData.InitializeAs<FMassTransformsSpawnData>();

        FMassTransformsSpawnData& SpawnData = Result.SpawnData.GetMutable<FMassTransformsSpawnData>();
        SpawnData.Transforms.Reserve(Result.NumEntities);

        for (int32 Index = 0; Index < Result.NumEntities; ++Index)
        {
            const float Angle = RandomStream.FRandRange(0.0f, UE_TWO_PI);
            const float Radius = FMath::Sqrt(RandomStream.FRand()) * SpawnRadius;
            const FVector Offset(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0f);

            FTransform& SpawnTransform = SpawnData.Transforms.AddDefaulted_GetRef();
            SpawnTransform.SetLocation(SpawnCenter + Offset);
        }
    }

    FinishedGeneratingSpawnPointsDelegate.Execute(Results);
}
