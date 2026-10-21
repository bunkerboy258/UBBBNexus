#include "BBBMonsterSpawnerPresentationActor.h"
#include "Components/SceneComponent.h"
#include "MassEntityConfigAsset.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Traits/BBBMonsterSpawnerTrait.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnInputFragment.h"

ABBBMonsterSpawnerPresentationActor::ABBBMonsterSpawnerPresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnerRoot"));
}

bool ABBBMonsterSpawnerPresentationActor::EnsureEntity()
{
    if (!HasAuthority() || !GetWorld() || GetWorld()->GetNetMode() == NM_Client || !GetWorld()->IsGameWorld() || !EntityConfig
        || EntityConfig->GetConfig().GetTraits().Num() != 1
        || !EntityConfig->GetConfig().FindTrait(UBBBMonsterSpawnerTrait::StaticClass(), true))
    {
        return false;
    }
    auto* Entities = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
    if (Entities && Entities->GetEntityManager().IsEntityValid(Entity))
    {
        return true;
    }
    Entity = GetWorld()->GetSubsystem<UBBBMassSubsystem>()->CreateEntity(*EntityConfig);
    return Entity.IsSet();
}

FBBBMonsterSpawnRegionDefinition ABBBMonsterSpawnerPresentationActor::ReadRegion() const
{
    auto Result = Region;
    Result.Transform = Region.Transform * GetActorTransform();
    return Result;
}

bool ABBBMonsterSpawnerPresentationActor::StartSpawning()
{
    if (!EnsureEntity())
    {
        return false;
    }
    auto& Manager = GetWorld()->GetSubsystem<UMassEntitySubsystem>()->GetMutableEntityManager();
    const auto* State = Manager.GetFragmentDataPtr<FBBBMonsterSpawnFragment>(Entity);
    const auto* Input = Manager.GetFragmentDataPtr<FBBBMonsterSpawnInputFragment>(Entity);
    if (!State || !Input || State->bActive || State->bCreating || Input->Request.bActive || !Definition)
    {
        return false;
    }
    FBBBMonsterSpawnRequestLocalControlPacket Packet;
    Packet.OwnerId = State->OwnerId.IsValid() ? State->OwnerId : FGuid::NewGuid();
    Packet.RequestId = State->RequestId + 1;
    Packet.Generation = State->Generation + 1;
    Packet.Definition = Definition;
    Packet.Regions.Add(ReadRegion());
    const int32 Seed = SeedOverride != 0 ? SeedOverride : Definition->Seed;
    Packet.Seed = Seed != 0 ? static_cast<uint32>(Seed) : (static_cast<uint32>(FMath::Rand()) | 1u);
    Packet.Count = Count;
    if (!Packet.IsValid())
    {
        return false;
    }
    return GetWorld()->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, MoveTemp(Packet));
}

bool ABBBMonsterSpawnerPresentationActor::PauseSpawning()
{
    return EnsureEntity() && GetWorld()->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, FBBBMonsterSpawnPauseLocalControlPacket());
}

bool ABBBMonsterSpawnerPresentationActor::ResumeSpawning()
{
    return EnsureEntity() && GetWorld()->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, FBBBMonsterSpawnResumeLocalControlPacket());
}

bool ABBBMonsterSpawnerPresentationActor::StopSpawning()
{
    return EnsureEntity() && GetWorld()->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, FBBBMonsterSpawnStopLocalControlPacket());
}

int32 ABBBMonsterSpawnerPresentationActor::ReadCreatedCount() const
{
    if (!GetWorld())
    {
        return 0;
    }
    const auto* Entities = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
    if (!Entities || !Entities->GetEntityManager().IsEntityValid(Entity))
    {
        return 0;
    }
    const auto* State = Entities->GetEntityManager().GetFragmentDataPtr<FBBBMonsterSpawnFragment>(Entity);
    return State ? State->Created : 0;
}

FString ABBBMonsterSpawnerPresentationActor::ReadStatus() const
{
    const auto* Entities = GetWorld() ? GetWorld()->GetSubsystem<UMassEntitySubsystem>() : nullptr;
    if (!Entities || !Entities->GetEntityManager().IsEntityValid(Entity))
    {
        return TEXT("尚未初始化");
    }
    const auto* State = Entities->GetEntityManager().GetFragmentDataPtr<FBBBMonsterSpawnFragment>(Entity);
    return State ? State->Reason : TEXT("缺少出生状态");
}

void ABBBMonsterSpawnerPresentationActor::BeginPlay()
{
    Super::BeginPlay();
    if (bAutoStart && HasAuthority())
    {
        StartSpawning();
    }
}

void ABBBMonsterSpawnerPresentationActor::EndPlay(const EEndPlayReason::Type Reason)
{
    if (auto* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>())
    {
        Mass->DestroyEntity(Entity);
    }
    Entity.Reset();
    Super::EndPlay(Reason);
}
