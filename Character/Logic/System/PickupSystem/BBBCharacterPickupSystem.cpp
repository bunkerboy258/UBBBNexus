#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/BBBCharacterPickupSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Context/BBBCharacterPickupUpdateContext.h"
void FBBBCharacterPickupSystem::Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData, const FBBBCharacterPickupConfig &InConfig)
{
    Character = &InCharacter;
    Data = &InData;
    Config = &InConfig;
}
void FBBBCharacterPickupSystem::Update()
{
    if (Character && Data && Config)
    {
        FBBBCharacterPickupUpdateContext Context{*Character, *Data, *Config};
        Processor.Update(Context);
    }
}
void FBBBCharacterPickupSystem::Shutdown()
{
    if (Character && Data && Config)
    {
        FBBBCharacterPickupUpdateContext Context{*Character, *Data, *Config};
        Processor.Shutdown(Context);
    }
}
