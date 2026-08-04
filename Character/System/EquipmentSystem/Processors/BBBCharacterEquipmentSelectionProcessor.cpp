#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
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

    if (EquipmentState.DesiredMainHandInstance
        && !ensureMsgf(EquipmentState.DesiredMainHandInstance->IsValid(), TEXT("[UBBBC]Desired equipment instance is invalid")))
    {
        return;
    }

    if (EquipmentState.ActiveMainHandInstance)
    {
        EquipmentState.ActiveMainHandInstance->ReleasePresentation();
    }

    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;

    if (EquipmentState.ActiveMainHandInstance)
    {
        EquipmentState.ActiveMainHandInstance->Equip(
            CharacterMesh,
            CharacterAPI,
            AttachmentSocketName);
    }
}
