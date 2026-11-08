#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/BBBGrenadeUseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeFinishProcessor.h"
void FBBBGrenadeUseSystem::Initialize(FBBBGrenadeRuntimeData &InData, const UBBBGrenadeDefinition &InDefinition,
                                      ABBBCharacter &InCharacter)
{
    Data = &InData;
    Definition = &InDefinition;
    Character = &InCharacter;
}
void FBBBGrenadeUseSystem::Update()
{
    if (!Data || !Definition || !Character)
    {
        return;
    }
    FBBBGrenadeUseUpdateContext Context{*Data, *Definition, *Character};
    FBBBGrenadeUseProcessor{}.Update(Context);
    FBBBGrenadeFinishProcessor{}.Update(Context);
}
void FBBBGrenadeUseSystem::Shutdown()
{
    Data = nullptr;
    Definition = nullptr;
    Character = nullptr;
}
