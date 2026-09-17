#include "BBBWork/UBBBNexus/MonsterMass/Spawning/MonsterSpawnPointsGenerator.h"

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
    // 没有生成数量时直接结束生成请求
    if (Count <= 0)
    {
        FinishedGeneratingSpawnPointsDelegate.Execute(TArray<FMassEntitySpawnDataGeneratorResult>());
        return;
    }

    // 生成位置必须来自演员拥有者
    const AActor* SpawnOwner = Cast<AActor>(&QueryOwner);

    if (!ensureMsgf(SpawnOwner != nullptr, TEXT("[UBBBM]Spawn point generator owner must be an actor")))
    {
        FinishedGeneratingSpawnPointsDelegate.Execute(TArray<FMassEntitySpawnDataGeneratorResult>());
        return;
    }

    // 按实体类型构建生成结果
    TArray<FMassEntitySpawnDataGeneratorResult> Results;
    BuildResultsFromEntityTypes(Count, EntityTypes, Results);

    // 以 MassSpawner 位置作为随机出生区域中心
    const FVector SpawnCenter = SpawnOwner->GetActorLocation();
    FRandomStream RandomStream(UE::Mass::Utils::OverrideRandomSeedForTesting(GetRandomSelectionSeed()));

    for (FMassEntitySpawnDataGeneratorResult& Result : Results)
    {
        // 使用引擎出生处理器写入批量 Transform
        Result.SpawnDataProcessor = UMassSpawnLocationProcessor::StaticClass();
        Result.SpawnData.InitializeAs<FMassTransformsSpawnData>();

        FMassTransformsSpawnData& SpawnData = Result.SpawnData.GetMutable<FMassTransformsSpawnData>();
        SpawnData.Transforms.Reserve(Result.NumEntities);

        for (int32 Index = 0; Index < Result.NumEntities; ++Index)
        {
            // 平方根半径保证圆盘内面积分布均匀
            const float Angle = RandomStream.FRandRange(0.0f, UE_TWO_PI);
            const float Radius = FMath::Sqrt(RandomStream.FRand()) * SpawnRadius;
            const FVector Offset(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0f);

            FTransform& SpawnTransform = SpawnData.Transforms.AddDefaulted_GetRef();
            SpawnTransform.SetLocation(SpawnCenter + Offset);
        }
    }

    FinishedGeneratingSpawnPointsDelegate.Execute(Results);
}
