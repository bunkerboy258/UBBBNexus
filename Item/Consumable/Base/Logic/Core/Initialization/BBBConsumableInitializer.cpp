#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Initialization/BBBConsumableInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "Components/StaticMeshComponent.h"
bool FBBBConsumableInitializer::Initialize(ABBBConsumable &Actor) const
{
    const auto *Definition = Actor.GetDefinition();
    if (!Definition || !Definition->Mesh || !Actor.GetMesh())
    {
        return false;
    }
    Actor.GetMesh()->SetStaticMesh(Definition->Mesh);
    Actor.GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Actor.GetMesh()->SetGenerateOverlapEvents(false);
    Actor.SetActorEnableCollision(false);
    return InitializeInstance(Actor);
}
