#include "BBBMonsterSpawnRegionPresentationActor.h"
#include "BBBMonsterSpawnRegionPresentationComponent.h"

ABBBMonsterSpawnRegionPresentationActor::ABBBMonsterSpawnRegionPresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    Region = CreateDefaultSubobject<UBBBMonsterSpawnRegionPresentationComponent>(TEXT("SpawnRegion"));
    RootComponent = Region;
}

FBBBMonsterSpawnRegionDefinition ABBBMonsterSpawnRegionPresentationActor::ReadDefinition() const
{
    return Region->ReadDefinition();
}

UBBBMonsterSpawnRegionPresentationComponent* ABBBMonsterSpawnRegionPresentationActor::GetRegionComponent() const
{
    return Region;
}
