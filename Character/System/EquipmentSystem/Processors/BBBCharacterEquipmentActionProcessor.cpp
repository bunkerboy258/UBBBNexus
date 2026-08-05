#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Results/BBBCharacterEquipmentResults.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"

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

    UBBBEquipmentSystem *EquipmentSystem = ActiveInstance->GetEquipmentSystem();
    if (!ensureMsgf(EquipmentSystem, TEXT("[UBBBC]Active equipment system is null")))
    {
        return;
    }

    EquipmentSystem->Update(CharacterAPI);

    if (EquipmentCommands.ConsumeFire() && EquipmentSystem->Fire(CharacterAPI))
    {
        EquipmentResults.RecordSuccessfulFire();
    }

    if (EquipmentCommands.ConsumeReload() && EquipmentSystem->Reload(CharacterAPI))
    {
        EquipmentResults.RecordSuccessfulReload();
    }

    if (EquipmentCommands.ConsumeFirePresentation())
    {
        EquipmentSystem->PresentFire(CharacterAPI);
    }

    if (EquipmentCommands.ConsumeReloadPresentation())
    {
        EquipmentSystem->PresentReload(CharacterAPI);
    }
}
