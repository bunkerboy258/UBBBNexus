#include "BBBMonsterSpawnRegionPresentationComponent.h"

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
    Result.Transform = GetComponentTransform();
    Result.Extent = GetUnscaledBoxExtent();
    Result.Shape = Shape;
    return Result;
}
