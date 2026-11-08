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
        Character->GetCharacterConfig().Item.WearableCatalog, Data->External.ReadNetworkIdentityState().bIsMirror};
    InputProcessor.Update(Context);
    SelectionProcessor.Update(Context);
    LoadProcessor.Update(Context);
    ResourceProcessor.Update(Context);
    MaterialProcessor.Update(Context);
    DisplayProcessor.Update(Context);
    ReadabilityProcessor.Update(Context);
}

void FBBBCharacterAppearanceSystem::Shutdown()
{
    if (!Character || !Data || !Config)
    {
        return;
    }
    FBBBCharacterAppearanceUpdateContext Context{*Character, *Data, *Config,
        Character->GetCharacterConfig().Item.WearableCatalog, Data->External.ReadNetworkIdentityState().bIsMirror};
    ReadabilityProcessor.Shutdown(Context);
    LoadProcessor.Shutdown(Context);
    DisplayProcessor.Shutdown(Context);
    InputProcessor.Shutdown(Context);
    Character = nullptr;
    Data = nullptr;
    Config = nullptr;
}
