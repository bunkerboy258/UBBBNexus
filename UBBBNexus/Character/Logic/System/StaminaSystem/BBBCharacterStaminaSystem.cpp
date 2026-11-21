#include "BBBWork/UBBBNexus/Character/Logic/System/StaminaSystem/BBBCharacterStaminaSystem.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/StaminaSystem/DomainData/Context/BBBCharacterStaminaUpdateContext.h"

void FBBBCharacterStaminaSystem::Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData,
                                            const UBBBCharacterConfig &InConfig)
{
    Character = &InCharacter;
    Data = &InData;
    Config = &InConfig;
}

void FBBBCharacterStaminaSystem::Update()
{
    if (!Character || !Data || !Config)
    {
        return;
    }
    FBBBCharacterStaminaUpdateContext Context{*Character, *Data, *Config};
    Processor.Update(Context);
}
