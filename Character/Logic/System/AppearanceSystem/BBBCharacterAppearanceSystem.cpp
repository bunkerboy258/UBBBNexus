#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/BBBCharacterAppearanceSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterAppearanceSystem::Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData,
    const FBBBCharacterAppearanceConfig &InConfig)
{
    Character = &InCharacter;
    Data = &InData;
    Config = &InConfig;
}

void FBBBCharacterAppearanceSystem::Update()
{
    if (!Character || !Data || !Config)
    {
        return;
    }
    FBBBCharacterAppearanceUpdateContext Context{*Character, *Data, *Config,
        Character->GetCharacterConfig().Item.Catalog, Data->External.ReadNetworkIdentityState().bIsMirror};
    InputProcessor.Update(Context);
    SelectionProcessor.Update(Context);
    ResourceProcessor.Update(Context);
    MaterialProcessor.Update(Context);
    DisplayProcessor.Update(Context);
}

void FBBBCharacterAppearanceSystem::Shutdown()
{
    if (!Character || !Data || !Config)
    {
        return;
    }
    FBBBCharacterAppearanceUpdateContext Context{*Character, *Data, *Config,
        Character->GetCharacterConfig().Item.Catalog, Data->External.ReadNetworkIdentityState().bIsMirror};
    DisplayProcessor.Shutdown(Context);
    InputProcessor.Shutdown(Context);
    Character = nullptr;
    Data = nullptr;
    Config = nullptr;
}
