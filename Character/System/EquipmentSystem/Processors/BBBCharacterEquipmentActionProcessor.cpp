#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"

void FBBBCharacterEquipmentActionProcessor::Advance(
    const float WorldTimeSeconds,
    FBBBCharacterEquipmentState &EquipmentState) const
{
    ABBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
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
    ABBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    FBBBCharacterEquipmentActionState &ActionState = EquipmentState.ActionState;

    TArray<FBBBEquipmentActionEvent> RestoredActions = EquipmentCommands.ConsumeRestoredActions();
    for (const FBBBEquipmentActionEvent &RestoredAction : RestoredActions)
    {
        if (!ActiveInstance || RestoredAction.EquipmentId != ActiveInstance->GetEquipmentId())
        {
            continue;
        }

        FBBBEquipmentActionResult Result;
        if (RestoredAction.ActionType == EBBBCharacterActionType::Equip)
        {
            if (ActionState.IsActive() || ActiveInstance->IsReloading())
            {
                continue;
            }

            if (!ActiveInstance->BeginEquipAction(
                RestoredAction.DurationSeconds,
                Result))
            {
                continue;
            }

            ActionState.Begin(
                EBBBCharacterActionType::Equip,
                WorldTimeSeconds,
                Result.DurationSeconds,
                RestoredAction.Sequence);
            PublishAction(EquipmentEvents, RestoredAction, Result);
            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Fire)
        {
            if (ActiveInstance->SubmitFire(Result))
            {
                PublishAction(EquipmentEvents, RestoredAction, Result);
            }

            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Reload
            && !ActionState.IsActive()
            && !ActiveInstance->IsReloading())
        {
            if (ActiveInstance->SubmitReload(
                WorldTimeSeconds,
                RestoredAction.Sequence,
                RestoredAction.DurationSeconds,
                Result))
            {
                PublishAction(EquipmentEvents, RestoredAction, Result);
            }
        }
    }

    const bool bShouldFire = EquipmentCommands.ConsumeFire();
    const bool bShouldReload = EquipmentCommands.ConsumeReload();
    if (!ActiveInstance || ActionState.IsActive() || ActiveInstance->IsReloading())
    {
        return;
    }

    if (bShouldFire)
    {
        FBBBEquipmentActionResult Result;
        const int32 Sequence = EquipmentState.NextActionSequence;
        if (ActiveInstance->SubmitFire(Result))
        {
            EquipmentState.NextActionSequence++;

            FBBBEquipmentActionEvent Event;
            Event.ActionType = EBBBCharacterActionType::Fire;
            Event.EquipmentId = ActiveInstance->GetEquipmentId();
            Event.Sequence = Sequence;
            PublishAction(EquipmentEvents, MoveTemp(Event), Result);
        }
    }

    if (bShouldReload)
    {
        FBBBEquipmentActionResult Result;
        const int32 Sequence = EquipmentState.NextActionSequence;
        if (ActiveInstance->SubmitReload(WorldTimeSeconds, Sequence, 0.0f, Result))
        {
            EquipmentState.NextActionSequence++;

            FBBBEquipmentActionEvent Event;
            Event.ActionType = EBBBCharacterActionType::Reload;
            Event.EquipmentId = ActiveInstance->GetEquipmentId();
            Event.Sequence = Sequence;
            PublishAction(EquipmentEvents, MoveTemp(Event), Result);
        }
    }
}

//------------------------------------------------------------------------------

void FBBBCharacterEquipmentActionProcessor::PublishAction(
    FBBBCharacterEquipmentEvents &EquipmentEvents,
    FBBBEquipmentActionEvent Event,
    const FBBBEquipmentActionResult &Result) const
{
    Event.DurationSeconds = Result.DurationSeconds;
    EquipmentEvents.AddAction(MoveTemp(Event));

    if (!Result.RecoilImpulse.IsNearlyZero())
    {
        FBBBEquipmentRecoilEvent Recoil;
        Recoil.Impulse = Result.RecoilImpulse;
        Recoil.RecoverySpeed = Result.RecoilRecoverySpeed;
        EquipmentEvents.AddRecoil(MoveTemp(Recoil));
    }
}
