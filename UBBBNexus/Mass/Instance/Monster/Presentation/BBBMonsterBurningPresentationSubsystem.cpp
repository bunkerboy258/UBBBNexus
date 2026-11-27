#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBurningPresentationSubsystem.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Misc/App.h"
#include "NiagaraDataChannel.h"
#include "NiagaraDataChannelAccessor.h"
#include "NiagaraDataChannelData.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraDataChannelHandler.h"

namespace
{
    struct FBurnBoneSpec
    {
        FName Bone;
        float SizeMin;
        float SizeMax;
        float Jitter;
    };

    /** 覆盖头、双肩、胸背、双臂与大腿的 8 骨骼采样表，消除居中结块与动作脱节 */
    const FBurnBoneSpec BurnBones[] = {
        {FName(TEXT("spine_03")),    28.0f, 44.0f, 10.0f},
        {FName(TEXT("spine_02")),    26.0f, 42.0f, 10.0f},
        {FName(TEXT("head")),        18.0f, 28.0f,  6.5f},
        {FName(TEXT("upperarm_l")),  20.0f, 32.0f,  7.0f},
        {FName(TEXT("upperarm_r")),  20.0f, 32.0f,  7.0f},
        {FName(TEXT("lowerarm_l")),  15.0f, 24.0f,  5.5f},
        {FName(TEXT("lowerarm_r")),  15.0f, 24.0f,  5.5f},
        {FName(TEXT("thigh_l")),     20.0f, 30.0f,  8.0f},
        {FName(TEXT("thigh_r")),     20.0f, 30.0f,  8.0f},
    };

    /** 按距离返回单只燃烧僵尸的每秒火焰点发射率 */
    float FlameRateForDistanceSq(const float DistSq)
    {
        if (DistSq <= FMath::Square(1000.0f))
        {
            return 22.0f;
        }
        if (DistSq <= FMath::Square(2200.0f))
        {
            return 13.0f;
        }
        if (DistSq <= FMath::Square(3600.0f))
        {
            return 6.5f;
        }
        return 0.0f;
    }

    FVector SampleBoneWorldPosition(const USkeletalMeshComponent& Mesh, const FBurnBoneSpec& Spec, FRandomStream& Random)
    {
        FVector Base = Mesh.GetComponentLocation() + FVector(0.0, 0.0, 90.0);
        if (Mesh.GetBoneIndex(Spec.Bone) != INDEX_NONE)
        {
            Base = Mesh.GetSocketLocation(Spec.Bone);
        }
        else if (Mesh.GetBoneIndex(FName(TEXT("spine_02"))) != INDEX_NONE)
        {
            Base = Mesh.GetSocketLocation(FName(TEXT("spine_02")));
        }
        return Base + FVector(
            Random.FRandRange(-Spec.Jitter, Spec.Jitter),
            Random.FRandRange(-Spec.Jitter, Spec.Jitter),
            Random.FRandRange(-Spec.Jitter * 0.65f, Spec.Jitter * 0.85f));
    }
}

void UBBBMonsterBurningPresentationSubsystem::Prepare()
{
    UWorld* World = GetWorld();
    if (!World || World->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return;
    }
    if (!BurningChannel)
    {
        BurningChannel = LoadObject<UNiagaraDataChannelAsset>(nullptr,
            TEXT("/Game/_Project/Mass/Monster/Effects/NDC_MonsterBurning.NDC_MonsterBurning"));
    }
    if (BurningChannel && !bPrepared)
    {
        UNiagaraDataChannelLibrary::GetNiagaraDataChannel(World, BurningChannel);
        bPrepared = true;
    }
}

void UBBBMonsterBurningPresentationSubsystem::QueueBurningBody(const USkeletalMeshComponent& BodyMesh,
    const FVector& Velocity, const uint32 VariationSeed, const float ViewerDistSq)
{
    if (ViewerDistSq > FMath::Square(3600.0f))
    {
        return;
    }
    QueuedBodies.Add(FBurningEntry{&BodyMesh, Velocity, VariationSeed, ViewerDistSq});
}

void UBBBMonsterBurningPresentationSubsystem::Flush(const float DeltaSeconds)
{
    UWorld* World = GetWorld();
    if (!World || World->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        QueuedBodies.Reset();
        return;
    }
    if (FlushedFrame == GFrameCounter)
    {
        QueuedBodies.Reset();
        return;
    }
    FlushedFrame = GFrameCounter;

    if (QueuedBodies.IsEmpty() || !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.0f)
    {
        QueuedBodies.Reset();
        if (SpawnAccumulators.Num() > 128)
        {
            SpawnAccumulators.Reset();
        }
        return;
    }

    Prepare();
    if (!BurningChannel)
    {
        QueuedBodies.Reset();
        return;
    }
    UNiagaraDataChannel* Channel = BurningChannel->Get();
    auto* Handler = Channel ? UNiagaraDataChannelLibrary::FindDataChannelHandler(World, Channel) : nullptr;
    if (!Channel || !Handler)
    {
        QueuedBodies.Reset();
        return;
    }

    const float ClampedDt = FMath::Min(DeltaSeconds, 0.05f);
    ++FrameSequence;

    // 按距离由近到远排序：超出全局硬预算时优先保障近距高表现，同时按比例温和降载而非截断消失
    QueuedBodies.Sort([](const FBurningEntry& A, const FBurningEntry& B)
    {
        return A.DistSq < B.DistSq;
    });

    float TotalDesiredRate = 0.0f;
    for (const FBurningEntry& Entry : QueuedBodies)
    {
        TotalDesiredRate += FlameRateForDistanceSq(Entry.DistSq);
    }
    // 全场景硬预算封顶：每秒至多 280 个火焰样本（无论同时烧 5 只还是 60 只僵尸，GPU 填充率绝对封顶）
    constexpr float GlobalMaxFlamesPerSecond = 280.0f;
    constexpr int32 HardMaxSamplesPerFrame = 32;
    const float BudgetScale = TotalDesiredRate > GlobalMaxFlamesPerSecond
        ? (GlobalMaxFlamesPerSecond / TotalDesiredRate) : 1.0f;

    struct FBurnSample
    {
        FVector Position;
        FVector Direction;
        FVector Normal;
        FVector4 Scale;
        int32 Surface;
        int32 Seed;
    };

    TArray<FBurnSample> Samples;
    Samples.Reserve(HardMaxSamplesPerFrame);
    TSet<uint32> SeenSeeds;
    SeenSeeds.Reserve(QueuedBodies.Num());

    for (const FBurningEntry& Entry : QueuedBodies)
    {
        const USkeletalMeshComponent* Mesh = Entry.Mesh.Get();
        if (!Mesh)
        {
            continue;
        }
        SeenSeeds.Add(Entry.Seed);
        const float Rate = FlameRateForDistanceSq(Entry.DistSq) * BudgetScale;
        if (Rate <= 0.0f)
        {
            continue;
        }
        float& Acc = SpawnAccumulators.FindOrAdd(Entry.Seed);
        Acc = FMath::Min(Acc + Rate * ClampedDt, 3.0f);
        const int32 EmitCount = FMath::FloorToInt(Acc);
        if (EmitCount <= 0)
        {
            continue;
        }
        Acc -= static_cast<float>(EmitCount);

        FRandomStream Random(static_cast<int32>(Entry.Seed ^ (FrameSequence * 2654435761u)));
        const FVector ClampedVel = Entry.Velocity.GetClampedToMaxSize(480.0f);
        const bool bCloseOrMid = Entry.DistSq <= FMath::Square(2200.0f);

        for (int32 EmitIdx = 0; EmitIdx < EmitCount && Samples.Num() < HardMaxSamplesPerFrame; ++EmitIdx)
        {
            const int32 BoneIdx = Random.RandRange(0, UE_ARRAY_COUNT(BurnBones) - 1);
            const FBurnBoneSpec& Spec = BurnBones[BoneIdx];

            FBurnSample Sample;
            Sample.Position = SampleBoneWorldPosition(*Mesh, Spec, Random);
            // 保留僵尸 45% 奔跑/踉跄惯性 + 向上热气流，形成真实贴身火尾
            Sample.Direction = ClampedVel * 0.45f + FVector(
                Random.FRandRange(-24.0f, 24.0f),
                Random.FRandRange(-24.0f, 24.0f),
                Random.FRandRange(55.0f, 115.0f));
            const float FlameSize = Random.FRandRange(Spec.SizeMin, Spec.SizeMax);
            const float Lifetime = Random.FRandRange(0.32f, 0.58f);
            // Normal 置零避免 Spawn 偏移骨骼位置，SprayScale.x/z 控制按骨骼缩放的火焰尺寸与寿命
            Sample.Normal = FVector::ZeroVector;
            Sample.Scale = FVector4(FlameSize / 36.0f, 1.0f, Lifetime / 0.55f, 1.0f);
            // Surface == 2 同时触发 1 颗上升火星（近中距约 55% 样本带火星，控制火星总量）
            Sample.Surface = (bCloseOrMid && Random.FRand() < 0.55f) ? 2 : 1;
            Sample.Seed = static_cast<int32>(Random.GetUnsignedInt());
            Samples.Add(Sample);
        }
    }

    // 清理已熄灭或离场的累积器，防止 Map 长期增长
    if (SpawnAccumulators.Num() > SeenSeeds.Num() + 32)
    {
        for (auto It = SpawnAccumulators.CreateIterator(); It; ++It)
        {
            if (!SeenSeeds.Contains(It.Key()))
            {
                It.RemoveCurrent();
            }
        }
    }

    QueuedBodies.Reset();
    if (Samples.IsEmpty())
    {
        return;
    }

    TMap<FNiagaraDataChannelData*, TArray<int32>> Batches;
    for (int32 Index = 0; Index < Samples.Num(); ++Index)
    {
        FNDCAccessContextInst Access(Channel->GetAccessContextType());
        Access.GetChecked<FNDCAccessContextLegacy>() = FNDCAccessContextLegacy(Samples[Index].Position);
        auto Data = Handler->FindData(Access, ENiagaraResourceAccess::WriteOnly);
        if (Data.IsValid())
        {
            Batches.FindOrAdd(Data.Get()).Add(Index);
        }
    }

    for (const auto& Batch : Batches)
    {
        FNDCAccessContextInst Access(Channel->GetAccessContextType());
        Access.GetChecked<FNDCAccessContextLegacy>() = FNDCAccessContextLegacy(Samples[Batch.Value[0]].Position);
        auto* Writer = UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel_WithContext(
            World, BurningChannel, Access, Batch.Value.Num(), false, true, false, TEXT("BBBMonsterBurning"));
        if (!Writer)
        {
            continue;
        }
        for (int32 Index = 0; Index < Batch.Value.Num(); ++Index)
        {
            const FBurnSample& S = Samples[Batch.Value[Index]];
            Writer->WritePosition(TEXT("Position"), Index, S.Position);
            Writer->WriteVector(TEXT("Direction"), Index, S.Direction);
            Writer->WriteVector(TEXT("Normal"), Index, S.Normal);
            Writer->WriteVector4(TEXT("SprayScale"), Index, S.Scale);
            Writer->WriteInt(TEXT("Surface"), Index, S.Surface);
            Writer->WriteInt(TEXT("ImpactSeed"), Index, S.Seed);
        }
    }
}

void UBBBMonsterBurningPresentationSubsystem::Deinitialize()
{
    QueuedBodies.Reset();
    SpawnAccumulators.Reset();
    BurningChannel = nullptr;
    bPrepared = false;
    FlushedFrame = MAX_uint64;
    Super::Deinitialize();
}
