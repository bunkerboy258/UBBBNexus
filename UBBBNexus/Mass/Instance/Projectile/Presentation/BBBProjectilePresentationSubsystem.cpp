#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"
#include "MassEntityConfigAsset.h"
#include "MassSpawnerSubsystem.h"
#include "MassSimulationSubsystem.h"
#include "MassProcessingPhaseManager.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "Misc/App.h"
#include "Engine/World.h"
#include "Subsystems/SubsystemCollection.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileImpact.h"

void UBBBProjectilePresentationSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    Collection.InitializeDependency<UMassSpawnerSubsystem>();
    Collection.InitializeDependency<UMassSimulationSubsystem>();
    Super::Initialize(Collection);
}

void UBBBProjectilePresentationSubsystem::Prepare(UBBBProjectileDefinition &Definition)
{
    if (!Prepared.Contains(&Definition))
    {
        Pending.AddUnique(&Definition);
    }
}

bool UBBBProjectilePresentationSubsystem::IsReady(const UNiagaraDataChannelAsset *Channel, const UNiagaraSystem *System) const
{
    return IsValid(Tracer) && ActiveChannel == Channel && ActiveSystem == System;
}

bool UBBBProjectilePresentationSubsystem::IsTickable() const
{
    return IsInitialized() && (!Pending.IsEmpty() || !PendingMeshes.IsEmpty() || !FragStreaks.IsEmpty() || (ActiveSystem && !IsValid(Tracer)));
}

TStatId UBBBProjectilePresentationSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UBBBProjectilePresentationSubsystem, STATGROUP_Tickables);
}

void UBBBProjectilePresentationSubsystem::PrepareTracer(UNiagaraDataChannelAsset *Channel, UNiagaraSystem *System)
{
    if (!Channel || !System || !FApp::CanEverRender() || GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }
    if (!ensureMsgf((!ActiveChannel || ActiveChannel == Channel) && (!ActiveSystem || ActiveSystem == System)
        && System->bRequireCurrentFrameData, TEXT("同一世界的弹丸必须共享当帧曳光通道与系统")))
    {
        return;
    }
    ActiveChannel = Channel;
    ActiveSystem = System;
}

void UBBBProjectilePresentationSubsystem::Tick(float DeltaTime)
{
    UpdateFragStreaks(DeltaTime);
    auto *World = GetWorld();
    auto *Spawner = World->GetSubsystem<UMassSpawnerSubsystem>();
    if (!Spawner || Spawner->GetEntityManagerChecked().IsProcessing())
    {
        return;
    }
    if (!Pending.IsEmpty())
    {
        auto *Definition = Pending[0].Get();
        if (Definition && Definition->EntityConfig)
        {
            Definition->EntityConfig->GetOrCreateEntityTemplate(*World);
            if (World->GetNetMode() != NM_DedicatedServer && FApp::CanEverRender())
            {
                if (Definition->ImpactChannel)
                {
                    UNiagaraDataChannelLibrary::GetNiagaraDataChannel(World, Definition->ImpactChannel);
                }
                PrepareTracer(Definition->PresentationChannel, Definition->PresentationSystem);
                if (Definition->Mesh && !Meshes.Contains(Definition->Mesh))
                {
                    PendingMeshes.AddUnique(Definition->Mesh);
                }
            }
            Prepared.Add(Definition);
        }
        Pending.RemoveAt(0);
    }
    PrepareNextMesh();
    auto *Simulation = World->GetSubsystem<UMassSimulationSubsystem>();
    if (World->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender() || !ActiveSystem || IsValid(Tracer)
        || !Simulation || !Simulation->IsSimulationStarted())
    {
        return;
    }
    UNiagaraDataChannelLibrary::GetNiagaraDataChannel(World, ActiveChannel);
    Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, ActiveSystem, FVector::ZeroVector,
        FRotator::ZeroRotator, FVector::OneVector, false, false, ENCPoolMethod::None, false);
    if (!Tracer)
    {
        return;
    }
    Tracer->SetForceSolo(true);
    Tracer->SetTickBehavior(ENiagaraTickBehavior::ForceTickLast);
    Tracer->PrimaryComponentTick.AddPrerequisite(Simulation,
        Simulation->GetMutablePhaseManager().GetProcessingPhaseTickFunction(EMassProcessingPhase::FrameEnd));
    Tracer->Activate();
}

void UBBBProjectilePresentationSubsystem::PrepareNextMesh()
{
    if (PendingMeshes.IsEmpty())
    {
        return;
    }
    auto *Mesh = PendingMeshes[0].Get();
    if (Meshes.Contains(Mesh))
    {
        PendingMeshes.RemoveAt(0);
        return;
    }
    if (Meshes.Num() >= 16)
    {
        for (auto It = Meshes.CreateIterator(); It; ++It)
        {
            if (It.Value()->GetInstanceCount() == 0)
            {
                It.Value()->DestroyComponent();
                It.RemoveCurrent();
                break;
            }
        }
        if (Meshes.Num() >= 16)
        {
            return;
        }
    }
    auto *Component = NewObject<UInstancedStaticMeshComponent>(GetWorld());
    Component->SetStaticMesh(Mesh);
    Component->SetMobility(EComponentMobility::Movable);
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetGenerateOverlapEvents(false);
    Component->SetCanEverAffectNavigation(false);
    Component->SetCastShadow(false);
    Component->PrimaryComponentTick.bCanEverTick = false;
    Component->RegisterComponentWithWorld(GetWorld());
    Component->PrecachePSOs();
    Meshes.Add(Mesh, Component);
    PendingMeshes.RemoveAt(0);
}

void UBBBProjectilePresentationSubsystem::UpdateMeshes(const TMap<UStaticMesh *, TArray<FTransform>> &Batches)
{
    for (const auto &Entry : Meshes)
    {
        if (!Batches.Contains(Entry.Key.Get()) && Entry.Value->GetInstanceCount() > 0)
        {
            Entry.Value->ClearInstances();
        }
    }
    for (const auto &Batch : Batches)
    {
        const auto *Found = Meshes.Find(Batch.Key);
        if (!Found)
        {
            PendingMeshes.AddUnique(Batch.Key);
            continue;
        }
        auto *Component = Found->Get();
        const int32 PreviousCount = Component->GetInstanceCount();
        const int32 CommonCount = FMath::Min(PreviousCount, Batch.Value.Num());
        if (CommonCount > 0)
        {
            Component->BatchUpdateInstancesTransforms(0, MakeArrayView(Batch.Value).Left(CommonCount), true, true, true);
        }
        if (PreviousCount > Batch.Value.Num())
        {
            TArray<int32> Removed;
            for (int32 Index = PreviousCount - 1; Index >= Batch.Value.Num(); --Index)
            {
                Removed.Add(Index);
            }
            Component->RemoveInstances(Removed, true);
        }
        if (Batch.Value.Num() > PreviousCount)
        {
            TArray<FTransform> Added;
            Added.Append(Batch.Value.GetData() + PreviousCount, Batch.Value.Num() - PreviousCount);
            Component->AddInstances(Added, false, true, false);
        }
    }
}

void UBBBProjectilePresentationSubsystem::PublishBulletHoles(TConstArrayView<FBBBProjectileImpact> Impacts)
{
    UWorld *World = GetWorld();
    if (!World || Impacts.IsEmpty() || World->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return;
    }
    if (!BulletHoleMaterial)
    {
        BulletHoleMaterial = LoadObject<UMaterialInterface>(nullptr,
            TEXT("/Game/_Project/Mass/Projectile/M_BBBProjectileBulletHole.M_BBBProjectileBulletHole"));
        if (!BulletHoleMaterial)
        {
            return;
        }
    }

    constexpr int32 MaxBulletHoles = 64;
    constexpr int32 MaxPerFrame = 16;
    constexpr double MinSpacingSq = 16.0;
    int32 Placed = 0;

    for (const FBBBProjectileImpact &Impact : Impacts)
    {
        if (Placed >= MaxPerFrame)
        {
            break;
        }
        if (!Impact.bStaticSurface || Impact.Surface == SurfaceType2)
        {
            continue;
        }
        bool bTooClose = false;
        for (const FVector &Existing : BulletHolePositions)
        {
            if (FVector::DistSquared(Existing, Impact.Position) < MinSpacingSq)
            {
                bTooClose = true;
                break;
            }
        }
        if (bTooClose)
        {
            continue;
        }

        UDecalComponent *Decal = nullptr;
        if (BulletHolePool.Num() < MaxBulletHoles)
        {
            Decal = NewObject<UDecalComponent>(World);
            Decal->SetDecalMaterial(BulletHoleMaterial);
            Decal->DecalSize = FVector(6.0, 6.5, 6.5);
            Decal->FadeScreenSize = 0.008f;
            Decal->SortOrder = 1;
            Decal->PrimaryComponentTick.bCanEverTick = false;
            Decal->RegisterComponentWithWorld(World);
            BulletHolePool.Add(Decal);
            BulletHolePositions.Add(Impact.Position);
        }
        else
        {
            const int32 Slot = NextBulletHoleSlot % MaxBulletHoles;
            NextBulletHoleSlot = (Slot + 1) % MaxBulletHoles;
            Decal = BulletHolePool[Slot].Get();
            BulletHolePositions[Slot] = Impact.Position;
        }

        if (IsValid(Decal))
        {
            const FVector Normal = Impact.Normal.GetSafeNormal(UE_SMALL_NUMBER, FVector::UpVector);
            const FQuat BaseRot = (-Normal).ToOrientationQuat();
            const FQuat RandomRoll(Normal, FMath::FRandRange(0.0f, UE_TWO_PI));
            const float SizeScale = FMath::FRandRange(0.82f, 1.22f);
            Decal->SetWorldLocationAndRotation(Impact.Position, RandomRoll * BaseRot);
            Decal->SetWorldScale3D(FVector(1.0f, SizeScale, SizeScale));
        }
        ++Placed;
    }
}

void UBBBProjectilePresentationSubsystem::SpawnFragStreaks(const FVector &Origin, TConstArrayView<FVector> Endpoints)
{
    UWorld *World = GetWorld();
    if (!World || Endpoints.IsEmpty() || World->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return;
    }
    if (!IsValid(FragStreakMesh))
    {
        UStaticMesh *Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
        UMaterialInterface *Mat = LoadObject<UMaterialInterface>(nullptr,
            TEXT("/Game/_Project/Mass/Projectile/M_Grenade_FragStreak.M_Grenade_FragStreak"));
        if (!Cylinder || !Mat)
        {
            return;
        }
        FragStreakMesh = NewObject<UInstancedStaticMeshComponent>(World);
        FragStreakMesh->SetStaticMesh(Cylinder);
        FragStreakMesh->SetMaterial(0, Mat);
        FragStreakMesh->SetMobility(EComponentMobility::Movable);
        FragStreakMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        FragStreakMesh->SetGenerateOverlapEvents(false);
        FragStreakMesh->SetCanEverAffectNavigation(false);
        FragStreakMesh->SetCastShadow(false);
        FragStreakMesh->PrimaryComponentTick.bCanEverTick = false;
        FragStreakMesh->RegisterComponentWithWorld(World);
    }

    constexpr int32 MaxActiveStreaks = 64;
    for (const FVector &End : Endpoints)
    {
        if (FragStreaks.Num() >= MaxActiveStreaks)
        {
            FragStreaks.RemoveAt(0);
        }
        if (FVector::DistSquared(Origin, End) < 400.0)
        {
            continue;
        }
        FFragStreak Streak;
        Streak.Origin = Origin;
        Streak.Endpoint = End;
        Streak.Age = 0.0f;
        Streak.Duration = FMath::FRandRange(0.075f, 0.125f);
        FragStreaks.Add(Streak);
    }
    UpdateFragStreaks(0.0f);
}

void UBBBProjectilePresentationSubsystem::UpdateFragStreaks(const float DeltaTime)
{
    if (!IsValid(FragStreakMesh))
    {
        FragStreaks.Reset();
        return;
    }
    if (FragStreaks.IsEmpty())
    {
        if (FragStreakMesh->GetInstanceCount() > 0)
        {
            FragStreakMesh->ClearInstances();
        }
        return;
    }

    TArray<FTransform> Transforms;
    Transforms.Reserve(FragStreaks.Num());
    for (int32 Index = FragStreaks.Num() - 1; Index >= 0; --Index)
    {
        FFragStreak &S = FragStreaks[Index];
        S.Age += DeltaTime;
        if (S.Age >= S.Duration)
        {
            FragStreaks.RemoveAtSwap(Index);
            continue;
        }
        const float Alpha = S.Age / S.Duration;
        // 头部迅速冲向落点 尾部稍慢跟进 形成高速白色破片划痕感
        const float HeadT = FMath::Clamp(Alpha * 2.2f, 0.08f, 1.0f);
        const float TailT = FMath::Clamp((Alpha - 0.15f) * 1.25f, 0.0f, HeadT - 0.04f);
        const FVector A = FMath::Lerp(S.Origin, S.Endpoint, TailT);
        const FVector B = FMath::Lerp(S.Origin, S.Endpoint, HeadT);
        const FVector Delta = B - A;
        const double Length = Delta.Size();
        if (Length < 4.0)
        {
            continue;
        }
        const FVector Dir = Delta / Length;
        const FQuat Rot = FQuat::FindBetweenNormals(FVector::UpVector, Dir);
        const double RadiusScale = FMath::Lerp(0.018, 0.004, Alpha);
        Transforms.Emplace(Rot, (A + B) * 0.5, FVector(RadiusScale, RadiusScale, Length / 100.0));
    }

    FragStreakMesh->ClearInstances();
    if (!Transforms.IsEmpty())
    {
        FragStreakMesh->AddInstances(Transforms, false, true, false);
    }
}

void UBBBProjectilePresentationSubsystem::Deinitialize()
{
    for (const auto &Decal : BulletHolePool)
    {
        if (IsValid(Decal))
        {
            Decal->DestroyComponent();
        }
    }
    BulletHolePool.Reset();
    BulletHolePositions.Reset();
    BulletHoleMaterial = nullptr;
    NextBulletHoleSlot = 0;
    if (IsValid(FragStreakMesh))
    {
        FragStreakMesh->DestroyComponent();
    }
    FragStreakMesh = nullptr;
    FragStreaks.Reset();
    if (IsValid(Tracer))
    {
        Tracer->DestroyComponent();
    }
    for (const auto &Mesh : Meshes)
    {
        if (IsValid(Mesh.Value))
        {
            Mesh.Value->DestroyComponent();
        }
    }
    Meshes.Reset();
    PendingMeshes.Reset();
    Tracer = nullptr;
    ActiveChannel = nullptr;
    ActiveSystem = nullptr;
    Pending.Reset();
    Prepared.Reset();
    Super::Deinitialize();
}
