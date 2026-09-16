#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

void FBBBCharacterEquipmentActionProcessor::Advance(
    const float WorldTimeSeconds,
    FBBBCharacterEquipmentState &EquipmentState) const
{
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    if (ActiveInstance)
    {
        ActiveInstance->AdvanceAction(WorldTimeSeconds);
    }

    FBBBCharacterEquipmentActionState &ActionState = EquipmentState.ActionState;
    if (ActionState.IsActive()
        && WorldTimeSeconds - ActionState.StartTimeSeconds >= ActionState.DurationSeconds)
    {
        ActionState.Reset();
    }
}

//------------------------------------------------------------------------------

void FBBBCharacterEquipmentActionProcessor::Update(
    const float WorldTimeSeconds,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterEquipmentEvents &EquipmentEvents) const
{
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    FBBBCharacterEquipmentActionState &ActionState = EquipmentState.ActionState;

    TArray<FBBBEquipmentActionEvent> RestoredActions = EquipmentCommands.ConsumeRestoredActions();
    for (const FBBBEquipmentActionEvent &RestoredAction : RestoredActions)
    {
        if (!ActiveInstance || !ActiveInstance->GetDefinition()
            || RestoredAction.EquipmentId != ActiveInstance->GetDefinition()->EquipmentId)
        {
            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Equip)
        {
            if (ActionState.IsActive() || ActiveInstance->IsReloading())
            {
                continue;
            }

            const float DurationSeconds = RestoredAction.DurationSeconds > 0.0f
                ? RestoredAction.DurationSeconds
                : ActiveInstance->GetEquipDuration();
            ActionState.Begin(
                EBBBCharacterActionType::Equip,
                WorldTimeSeconds,
                DurationSeconds,
                RestoredAction.Sequence);

            FBBBEquipmentActionEvent PresentedAction = RestoredAction;
            PresentedAction.DurationSeconds = DurationSeconds;
            ActiveInstance->BuildEquipActionPresentation(PresentedAction.Presentation);
            EquipmentEvents.AddAction(MoveTemp(PresentedAction));
            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Fire)
        {
            ActiveInstance->ApplyRestoredAction(RestoredAction, WorldTimeSeconds);
            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Reload
            && !ActionState.IsActive()
            && !ActiveInstance->IsReloading())
        {
            ActiveInstance->ApplyRestoredAction(RestoredAction, WorldTimeSeconds);
        }
    }

    const bool bShouldFire = EquipmentCommands.ConsumeFire();
    const bool bShouldReload = EquipmentCommands.ConsumeReload();
    if (ActiveInstance && !ActionState.IsActive() && !ActiveInstance->IsReloading())
    {
        if (bShouldFire && ActiveInstance->SubmitFire(EquipmentState.NextActionSequence))
        {
            EquipmentState.NextActionSequence++;
        }

        if (bShouldReload && ActiveInstance->SubmitReload(
            WorldTimeSeconds,
            EquipmentState.NextActionSequence))
        {
            EquipmentState.NextActionSequence++;
        }
    }

    if (ActiveInstance)
    {
        ActiveInstance->ConsumeEvents(EquipmentEvents);
    }
}
