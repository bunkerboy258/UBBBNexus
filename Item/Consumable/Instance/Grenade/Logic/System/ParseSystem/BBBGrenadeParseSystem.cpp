#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/BBBGrenadeParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/Context/BBBGrenadeParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBGrenadeParseSystem::Initialize(ABBBGrenadeConsumable &InActor, FBBBGrenadeRuntimeData &InData)
{
    Actor = &InActor;
    Data = &InData;
}

void FBBBGrenadeParseSystem::Update()
{
    ABBBCharacter *Character = Actor ? Cast<ABBBCharacter>(Actor->GetOwner()) : nullptr;
    if (!Actor || !Data || !Character)
    {
        return;
    }
    FBBBGrenadeParseUpdateContext Context{Data->Parse.InputState, Data->Use.UseInputState,
        Data->Use.UseState, Character->IsNetworkMirror()};
    ParseProcessor.Update(Context);
}
