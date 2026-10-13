#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/BBBGrenadeUseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBGrenadeUseSystem::Initialize(ABBBGrenadeConsumable &InActor, FBBBGrenadeRuntimeData &InData)
{
    Actor = &InActor;
    Data = &InData;
}

void FBBBGrenadeUseSystem::Update()
{
    ABBBCharacter *Character = Actor ? Cast<ABBBCharacter>(Actor->GetOwner()) : nullptr;
    if (!Actor || !Data || !Character)
    {
        return;
    }
    const auto *Definition = Cast<UBBBGrenadeDefinition>(Actor->GetDefinition());
    if (!Definition || !Actor->GetWorld())
    {
        return;
    }
    FBBBGrenadeUseUpdateContext Context{*Actor, *Character, *Actor->GetWorld(), *Data,
        *Definition, Character->IsNetworkMirror()};
    UseProcessor.Update(Context);
    LaunchProcessor.Update(Context);
    FinishProcessor.Update(Context);
}
