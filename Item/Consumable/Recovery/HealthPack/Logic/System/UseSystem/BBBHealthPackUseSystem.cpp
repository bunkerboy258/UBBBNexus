#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/BBBHealthPackUseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/DomainData/Context/BBBHealthPackUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/Processors/BBBHealthPackUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/Processors/BBBHealthPackFinishProcessor.h"
void FBBBHealthPackUseSystem::Initialize(FBBBHealthPackRuntimeData &InData,
                                         const UBBBHealthPackDefinition &InDefinition, ABBBCharacter &InCharacter)
{
    Data = &InData;
    Definition = &InDefinition;
    Character = &InCharacter;
}
void FBBBHealthPackUseSystem::Update()
{
    if (!Data || !Definition || !Character)
    {
        return;
    }
    FBBBHealthPackUseUpdateContext Context{*Data, *Definition, *Character};
    FBBBHealthPackUseProcessor{}.Update(Context);
    FBBBHealthPackFinishProcessor{}.Update(Context);
}
void FBBBHealthPackUseSystem::Shutdown()
{
    Data = nullptr;
    Definition = nullptr;
    Character = nullptr;
}
