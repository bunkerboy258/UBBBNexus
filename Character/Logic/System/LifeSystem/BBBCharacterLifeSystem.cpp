#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/BBBCharacterLifeSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"

void FBBBCharacterLifeSystem::Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData,
                                         const UBBBCharacterConfig &InConfig)
{
    Character = &InCharacter;
    Data = &InData;
    Config = &InConfig;
    FBBBCharacterLifeUpdateContext Context{*Character, *Data, *Config};
    Processor.Initialize(Context);
}

void FBBBCharacterLifeSystem::Update()
{
    if (!Character || !Data || !Config)
    {
        return;
    }
    FBBBCharacterLifeUpdateContext Context{*Character, *Data, *Config};
    Processor.Update(Context);
}
