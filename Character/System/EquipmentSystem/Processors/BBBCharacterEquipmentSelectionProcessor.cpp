#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    ABBBCharacter &Character,
    USkeletalMeshComponent &CharacterMesh,
    const FName AttachmentSocketName,
    const float WorldTimeSeconds,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterEquipmentEvents &EquipmentEvents) const
{
    bool bRestoringEquipment = false;
    UBBBEquipmentDefinition *RestoredDefinition = EquipmentCommands.ConsumeRestoredEquipment();
    if (RestoredDefinition)
    {
        UBBBEquipmentInstance *RestoredInstance = UBBBEquipmentInstance::Create(
            Character,
            *RestoredDefinition);
        if (!ensureMsgf(RestoredInstance, TEXT("[UBBBC]Restored equipment instance creation failed")))
        {
            return;
        }

        if (!RestoredInstance->BindHolder(CharacterMesh, AttachmentSocketName))
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

    if (EquipmentState.ActionState.IsActive()
        || (EquipmentState.ActiveMainHandInstance && EquipmentState.ActiveMainHandInstance->IsReloading()))
    {
        return;
    }

    if (EquipmentState.ActiveMainHandInstance)
    {
        EquipmentState.ActiveMainHandInstance->Deactivate();
    }

    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;
    UBBBEquipmentInstance *DesiredInstance = EquipmentState.ActiveMainHandInstance;
    if (!DesiredInstance)
    {
        return;
    }

    if (!DesiredInstance->BindHolder(CharacterMesh, AttachmentSocketName)
        || !DesiredInstance->Activate())
    {
        EquipmentState.ActiveMainHandInstance = nullptr;
        return;
    }

    UBBBEquipmentDefinition *Definition = DesiredInstance->GetDefinition();
    if (!ensureMsgf(Definition, TEXT("[UBBBC]Active equipment definition is null after equip")))
    {
        DesiredInstance->Deactivate();
        EquipmentState.ActiveMainHandInstance = nullptr;
        return;
    }

    if (bRestoringEquipment)
    {
        return;
    }

    const int32 Sequence = EquipmentState.NextActionSequence++;
    const float DurationSeconds = DesiredInstance->GetEquipDuration();
    EquipmentState.ActionState.Begin(
        EBBBCharacterActionType::Equip,
        WorldTimeSeconds,
        DurationSeconds,
        Sequence);

    FBBBEquipmentActionEvent Event;
    Event.ActionType = EBBBCharacterActionType::Equip;
    Event.EquipmentId = Definition->EquipmentId;
    Event.Sequence = Sequence;
    Event.DurationSeconds = DurationSeconds;
    DesiredInstance->BuildEquipActionPresentation(Event.Presentation);
    EquipmentEvents.AddAction(MoveTemp(Event));
}
