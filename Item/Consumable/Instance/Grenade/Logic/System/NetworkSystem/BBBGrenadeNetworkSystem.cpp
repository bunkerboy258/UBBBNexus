#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/BBBGrenadeNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/DomainData/Context/BBBGrenadeNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBGrenadeNetworkSystem::Initialize(ABBBGrenadeConsumable &InActor, FBBBGrenadeRuntimeData &InData)
{
    Actor = &InActor;
    Data = &InData;
}

void FBBBGrenadeNetworkSystem::Update()
{
    ABBBCharacter *Character = Actor ? Cast<ABBBCharacter>(Actor->GetOwner()) : nullptr;
    if (!Actor || !Data || !Character)
    {
        return;
    }
    FBBBGrenadeNetworkUpdateContext Context{*Actor, *Character, *Data};
    NetworkProcessor.Update(Context);
}
