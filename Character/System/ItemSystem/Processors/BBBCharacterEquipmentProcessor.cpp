#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterEquipmentProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/States/BBBCharacterItemStates.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
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

        if (EquipmentState.ActiveMainHandInstance)
        {
            EquipmentState.ActiveMainHandInstance->Equip(
                CharacterMesh,
                CharacterAPI,
                AttachmentSocketName);
        }
    }

}
