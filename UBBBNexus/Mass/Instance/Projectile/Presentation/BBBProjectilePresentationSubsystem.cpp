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
#include "Engine/StaticMesh.h"

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
    return IsInitialized() && (!Pending.IsEmpty() || !PendingMeshes.IsEmpty() || (ActiveSystem && !IsValid(Tracer)));
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

void UBBBProjectilePresentationSubsystem::Tick(float)
{
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

void UBBBProjectilePresentationSubsystem::Deinitialize()
{
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
