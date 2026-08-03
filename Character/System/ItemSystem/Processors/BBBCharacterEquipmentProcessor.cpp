#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterEquipmentProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    float WorldTimeSeconds,
    FName AttachmentSocketName,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterExternalAPI &CharacterAPI) const
{
    if (EquipmentState.ActiveMainHandInstance != EquipmentState.DesiredMainHandInstance)
    {
        if (EquipmentState.ActiveMainHandInstance)
        {
            EquipmentState.ActiveMainHandInstance->ReleaseModel();
        }

        EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;
    }

    if (!EquipmentState.ActiveMainHandInstance)
    { return; }

    EquipmentState.ActiveMainHandInstance->Initialize(
        CharacterMesh,
        CharacterAPI,
        AttachmentSocketName,
        WorldTimeSeconds);

    EquipmentState.ActiveMainHandInstance->Update(WorldTimeSeconds);
}
