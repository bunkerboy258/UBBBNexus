#include "BBBMonsterSpawnRegionPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnDefinition.h"

UBBBMonsterSpawnRegionPresentationComponent::UBBBMonsterSpawnRegionPresentationComponent()
{
    InitBoxExtent(FVector(1000.0, 1000.0, 300.0));
    SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetGenerateOverlapEvents(false);
    SetHiddenInGame(true);
    bDrawOnlyIfSelected = true;
}

FBBBMonsterSpawnRegionDefinition UBBBMonsterSpawnRegionPresentationComponent::ReadDefinition() const
{
    FBBBMonsterSpawnRegionDefinition Result;
    Result.Id = RegionId;
    Result.Role = Role;
    Result.Transform = GetComponentTransform();
    Result.Extent = GetUnscaledBoxExtent();
    Result.Shape = Shape;
    Result.PopulationCount = PopulationCount;
    Result.ActivationRadius = ActivationRadius;
    Result.SpawnDefinitionOverride = SpawnDefinitionOverride;
    return Result;
}
