#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Results/BBBCharacterEquipmentResults.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

void FBBBCharacterEquipmentActionProcessor::Update(
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterEquipmentResults &EquipmentResults,
    FBBBCharacterExternalAPI &CharacterAPI) const
{
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    if (!ActiveInstance)
    {
        return;
    }

    ActiveInstance->Update(CharacterAPI);

    if (EquipmentCommands.ConsumeFire() && ActiveInstance->Fire(CharacterAPI))
    {
        EquipmentResults.RecordSuccessfulFire();
    }

    if (EquipmentCommands.ConsumeReload() && ActiveInstance->Reload(CharacterAPI))
    {
        EquipmentResults.RecordSuccessfulReload();
    }

    if (EquipmentCommands.ConsumeFirePresentation())
    {
        ActiveInstance->PresentFire(CharacterAPI);
    }

    if (EquipmentCommands.ConsumeReloadPresentation())
    {
        ActiveInstance->PresentReload(CharacterAPI);
    }
}
