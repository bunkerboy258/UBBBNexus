#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    ABBBCharacterInstance &Character,
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
        ABBBEquipmentInstance *RestoredInstance = ABBBEquipmentInstance::Create(
            Character,
            *RestoredDefinition,
            true);
        if (!ensureMsgf(RestoredInstance, TEXT("[UBBBC]Restored equipment instance creation failed")))
        {
            return;
        }

        if (!RestoredInstance->BindHolder(CharacterMesh, AttachmentSocketName))
        {
            RestoredInstance->Shutdown();
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
    ABBBEquipmentInstance *DesiredInstance = EquipmentState.ActiveMainHandInstance;
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

    if (bRestoringEquipment)
    {
        return;
    }

    const int32 Sequence = EquipmentState.NextActionSequence++;
    FBBBEquipmentActionResult Result;
    if (!DesiredInstance->BeginEquipAction(Sequence, 0.0f, Result))
    {
        return;
    }

    EquipmentState.ActionState.Begin(
        EBBBCharacterActionType::Equip,
        WorldTimeSeconds,
        Result.DurationSeconds,
        Sequence);

    FBBBEquipmentActionEvent Event;
    Event.ActionType = EBBBCharacterActionType::Equip;
    Event.EquipmentId = DesiredInstance->GetEquipmentId();
    Event.Sequence = Sequence;
    Event.DurationSeconds = Result.DurationSeconds;
    Event.Presentation = Result.Presentation;
    EquipmentEvents.AddAction(MoveTemp(Event));
}
