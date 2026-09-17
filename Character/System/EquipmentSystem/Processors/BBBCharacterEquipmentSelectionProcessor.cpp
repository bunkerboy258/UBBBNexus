#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    ABBBCharacterInstance &Character,
    USkeletalMeshComponent &CharacterMesh,
    const FName AttachmentSocketName,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterEquipmentEvents &EquipmentEvents) const
{
    bool bRestoringEquipment = false;
    UBBBEquipmentDefinition *RestoredDefinition = EquipmentCommands.ConsumeRestoredEquipment();
    if (RestoredDefinition)
    {
        ABBBEquipmentInstance *RestoredInstance = FBBBCharacterEquipmentLifecycleProcessor::Create(
            Character,
            *RestoredDefinition,
            true);
        if (!ensureMsgf(RestoredInstance, TEXT("[UBBBC]Restored equipment instance creation failed")))
        {
            return;
        }

        EquipmentState.DesiredMainHandInstance = RestoredInstance;
        bRestoringEquipment = true;
    }

    if (EquipmentState.ActiveMainHandInstance == EquipmentState.DesiredMainHandInstance)
    {
        return;
    }

    if (EquipmentState.ActiveMainHandInstance)
    {
        if (bRestoringEquipment)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(&CharacterMesh, *EquipmentState.ActiveMainHandInstance);
        }
        if (!bRestoringEquipment)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Detach(&CharacterMesh, *EquipmentState.ActiveMainHandInstance);
        }
    }

    EquipmentState.ReloadSequence = INDEX_NONE;
    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;
    ABBBEquipmentInstance *DesiredInstance = EquipmentState.ActiveMainHandInstance;
    if (!DesiredInstance)
    {
        return;
    }

    if (!FBBBCharacterEquipmentLifecycleProcessor::Attach(CharacterMesh, AttachmentSocketName, *DesiredInstance))
    {
        FBBBCharacterEquipmentLifecycleProcessor::Detach(&CharacterMesh, *DesiredInstance);
        EquipmentState.ActiveMainHandInstance = nullptr;
        return;
    }

    if (bRestoringEquipment)
    {
        return;
    }

    DesiredInstance->GetExternalAPI().SubmitEquip(EquipmentState.NextActionSequence++);
}
