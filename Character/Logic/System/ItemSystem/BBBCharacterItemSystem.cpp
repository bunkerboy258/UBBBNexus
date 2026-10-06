#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/BBBCharacterItemSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterItemSystem::Initialize(ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InRuntimeData, const FBBBCharacterItemConfig &Config)
{
    Character = &InCharacter;
    RuntimeData = &InRuntimeData;
    ItemConfig = &Config;
}

void FBBBCharacterItemSystem::Update()
{
    if (!Character || !RuntimeData || !ItemConfig)
    {
        return;
    }
    FBBBCharacterItemUpdateContext Context{*Character, *RuntimeData, *ItemConfig};
    InventoryProcessor.Update(Context);
    AcquisitionProcessor.Update(Context);
    BarProcessor.Update(Context);
}

void FBBBCharacterItemSystem::Shutdown()
{
    if (!Character || !RuntimeData || !ItemConfig)
    {
        return;
    }
    FBBBCharacterItemUpdateContext Context{*Character, *RuntimeData, *ItemConfig};
    AcquisitionProcessor.Shutdown(Context);
    BarProcessor.Shutdown(Context);
}
