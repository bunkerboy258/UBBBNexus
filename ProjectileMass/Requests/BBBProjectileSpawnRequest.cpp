#include "BBBWork/UBBBNexus/ProjectileMass/Requests/BBBProjectileSpawnRequest.h"

#include "BBBWork/UBBBNexus/ProjectileMass/Config/BBBProjectileDefinition.h"

bool FBBBProjectileSpawnRequest::IsValid() const
{
    return Definition != nullptr
        && Definition->IsValid()
        && MuzzleTransform.IsValid()
        && DamageCauser.IsValid()
        && InstigatorPawn.IsValid();
}
