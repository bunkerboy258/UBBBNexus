#include "BBBMassNavigationQueryActor.h"

ABBBMassNavigationQueryActor::ABBBMassNavigationQueryActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void ABBBMassNavigationQueryActor::SetTraversalLimit(const float Height)
{
    check(IsInGameThread());
    MaximumHeight = FMath::IsFinite(Height) ? FMath::Max(Height, 0.0f) : 0.0f;
}
