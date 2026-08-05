#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    FName AttachmentSocketName,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterExternalAPI &CharacterAPI) const
{
    if (EquipmentState.ActiveMainHandInstance == EquipmentState.DesiredMainHandInstance)
    {
        return;
    }

    UBBBEquipmentSystem *DesiredEquipmentSystem = EquipmentState.DesiredMainHandInstance
        ? EquipmentState.DesiredMainHandInstance->GetEquipmentSystem()
        : nullptr;

    if (EquipmentState.DesiredMainHandInstance
        && !ensureMsgf(DesiredEquipmentSystem, TEXT("[UBBBC]Desired equipment system is null")))
    {
        return;
    }

    if (EquipmentState.ActiveMainHandInstance)
    {
        UBBBEquipmentSystem *ActiveEquipmentSystem = EquipmentState.ActiveMainHandInstance->GetEquipmentSystem();
        if (!ensureMsgf(ActiveEquipmentSystem, TEXT("[UBBBC]Active equipment system is null")))
        {
            return;
        }

        ActiveEquipmentSystem->ReleasePresentation();
    }

    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;

    if (EquipmentState.ActiveMainHandInstance)
    {
        DesiredEquipmentSystem->Equip(
            CharacterMesh,
            CharacterAPI,
            AttachmentSocketName);
    }
}
