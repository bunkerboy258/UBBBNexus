#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Shutdown/BBBConsumableShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "Components/StaticMeshComponent.h"
void FBBBConsumableShutdown::Shutdown(ABBBConsumable &Actor) const
{
    ShutdownInstance(Actor);
    Actor.SetActorTickEnabled(false);
    Actor.bInitialized = false;
}
