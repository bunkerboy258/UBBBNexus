#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Initialization/BBBGrenadeInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"
#include "Components/StaticMeshComponent.h"

bool FBBBGrenadeInitializer::InitializeInstance(ABBBConsumable &BaseActor) const
{
    auto &Actor = static_cast<ABBBGrenadeConsumable &>(BaseActor);
    const auto *Definition = Cast<UBBBGrenadeDefinition>(Actor.GetDefinition());
    if (!Definition || !Definition->Mesh || !Definition->Projectile || !Definition->Projectile->IsValid()
        || Definition->Projectile->ExplosionRadiusCm <= 0.0f || Definition->Projectile->FuseSeconds <= 0.0f)
    {
        return false;
    }
    Actor.ParseSystem.Initialize(Actor, Actor.RuntimeData);
    Actor.UseSystem.Initialize(Actor, Actor.RuntimeData);
    Actor.NetworkSystem.Initialize(Actor, Actor.RuntimeData);
    return true;
}
