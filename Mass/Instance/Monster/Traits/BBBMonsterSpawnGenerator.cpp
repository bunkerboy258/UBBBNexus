#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterSpawnGenerator.h"

#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "MassCommonUtils.h"
#include "MassSpawnLocationProcessor.h"
#include "MassSpawnerTypes.h"

void UBBBMonsterSpawnGenerator::Generate(
    UObject& QueryOwner,
    TConstArrayView<FMassSpawnedEntityType> EntityTypes,
    const int32 Count,
    FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const
{
    // 没有生成数量时直接结束生成请求
    if (Count <= 0 || QueryOwner.GetWorld()->GetNetMode() == NM_Client)
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
    FRandomStream RandomStream(UE::Mass::Utils::OverrideRandomSeedForTesting(GetRandomSelectionSeed()));
    TArray<float, TInlineAllocator<16>> Weights;
    TArray<int32, TInlineAllocator<16>> Counts;
    Counts.SetNumZeroed(EntityTypes.Num());
    float TotalWeight = 0.0f;
    for (const FMassSpawnedEntityType& Type : EntityTypes)
    {
        const float Weight = FMath::IsFinite(Type.Proportion) && Type.Proportion > 0.0f && Type.GetEntityConfig() ? Type.Proportion : 0.0f;
        TotalWeight += Weight;
        Weights.Add(TotalWeight);
    }
    if (!ensureMsgf(TotalWeight > 0.0f && FMath::IsFinite(TotalWeight), TEXT("[BBBMonsterVariation]No valid weighted appearance templates")))
    {
        FinishedGeneratingSpawnPointsDelegate.Execute(Results);
        return;
    }
    for (int32 Index = 0; Index < Count; ++Index)
    {
        const float Selection = RandomStream.FRand() * TotalWeight;
        for (int32 Type = 0; Type < Weights.Num(); ++Type)
        {
            if (Selection < Weights[Type])
            {
                ++Counts[Type];
                break;
            }
        }
    }
    for (int32 Type = 0; Type < Counts.Num(); ++Type)
    {
        if (Counts[Type] > 0)
        {
            auto& Result = Results.AddDefaulted_GetRef();
            Result.EntityConfigIndex = Type;
            Result.NumEntities = Counts[Type];
        }
    }

    // 以 MassSpawner 位置作为随机出生区域中心
    const FVector SpawnCenter = SpawnOwner->GetActorLocation();

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
