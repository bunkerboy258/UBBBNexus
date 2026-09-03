#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireResults.h"
#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"

namespace
{
    FName GetEquipmentId(const UBBBEquipmentInstance &Instance)
    {
        const UBBBEquipmentDefinition *Definition = Instance.GetDefinition();
        return Definition
            ? Definition->EquipmentId
            : NAME_None;
    }
}

void FBBBCharacterEquipmentActionProcessor::Update(
    const float WorldTimeSeconds,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterEquipmentEvents &EquipmentEvents) const
{
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    UBBBEquipmentSystem *EquipmentSystem = ActiveInstance
        ? ActiveInstance->GetEquipmentSystem()
        : nullptr;

    FBBBCharacterEquipmentActionState &ActionState = EquipmentState.ActionState;
    if (ActionState.IsActive())
    {
        if (!ensureMsgf(EquipmentSystem, TEXT("[UBBBC]Persistent equipment action has no active equipment system")))
        {
            ActionState.Reset();
        }
    }

    if (ActionState.IsActive())
    {
        const float ElapsedSeconds = FMath::Max(WorldTimeSeconds - ActionState.StartTimeSeconds, 0.0f);
        const float NormalizedTime = FMath::Clamp(ElapsedSeconds / ActionState.DurationSeconds, 0.0f, 1.0f);

        if (ActionState.ActiveAction == EBBBCharacterActionType::Reload)
        {
            if (!ActionState.bMagazineRemoved
                && NormalizedTime >= ActionState.MagazineRemoveNormalizedTime)
            {
                EquipmentSystem->RemoveMagazine();
                ActionState.bMagazineRemoved = true;
            }

            if (!ActionState.bMagazineSpawned
                && NormalizedTime >= ActionState.MagazineSpawnNormalizedTime)
            {
                EquipmentSystem->SpawnMagazine();
                ActionState.bMagazineSpawned = true;
            }
        }

        if (NormalizedTime >= 1.0f)
        {
            if (ActionState.ActiveAction == EBBBCharacterActionType::Reload)
            {
                EquipmentSystem->CommitReload();
            }

            ActionState.Reset();
        }
    }

    TArray<FBBBEquipmentActionEvent> RestoredActions = EquipmentCommands.ConsumeRestoredActions();
    for (FBBBEquipmentActionEvent &RestoredAction : RestoredActions)
    {
        if (!EquipmentSystem)
        {
            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Fire)
        {
            EquipmentSystem->PresentFire();
            EquipmentEvents.AddAction(MoveTemp(RestoredAction));
            continue;
        }

        if (ActionState.IsActive())
        {
            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Equip)
        {
            const float DurationSeconds = RestoredAction.DurationSeconds > 0.0f
                ? RestoredAction.DurationSeconds
                : EquipmentSystem->GetEquipDuration();
            ActionState.Begin(
                EBBBCharacterActionType::Equip,
                WorldTimeSeconds,
                DurationSeconds,
                RestoredAction.Sequence);
            RestoredAction.DurationSeconds = DurationSeconds;
            EquipmentEvents.AddAction(MoveTemp(RestoredAction));
            continue;
        }

        if (RestoredAction.ActionType == EBBBCharacterActionType::Reload)
        {
            const float DurationSeconds = RestoredAction.DurationSeconds > 0.0f
                ? RestoredAction.DurationSeconds
                : EquipmentSystem->GetReloadDuration();
            ActionState.Begin(
                EBBBCharacterActionType::Reload,
                WorldTimeSeconds,
                DurationSeconds,
                RestoredAction.Sequence,
                EquipmentSystem->GetMagazineRemoveNormalizedTime(),
                EquipmentSystem->GetMagazineSpawnNormalizedTime());
            RestoredAction.DurationSeconds = DurationSeconds;
            EquipmentEvents.AddAction(MoveTemp(RestoredAction));
        }
    }

    const bool bShouldFire = EquipmentCommands.ConsumeFire();
    const bool bShouldReload = EquipmentCommands.ConsumeReload();
    if (!EquipmentSystem || ActionState.IsActive())
    {
        return;
    }

    if (bShouldFire)
    {
        FBBBEquipmentFireResult FireResult;
        if (EquipmentSystem->Fire(FireResult))
        {
            const int32 Sequence = EquipmentState.NextActionSequence++;

            FBBBEquipmentActionEvent ActionEvent;
            ActionEvent.ActionType = EBBBCharacterActionType::Fire;
            ActionEvent.EquipmentId = GetEquipmentId(*ActiveInstance);
            ActionEvent.Sequence = Sequence;
            EquipmentEvents.AddAction(MoveTemp(ActionEvent));

            FBBBEquipmentRecoilEvent RecoilEvent;
            RecoilEvent.Impulse = FireResult.RecoilImpulse;
            RecoilEvent.RecoverySpeed = FireResult.RecoilRecoverySpeed;
            EquipmentEvents.AddRecoil(MoveTemp(RecoilEvent));
        }
    }

    if (bShouldReload && EquipmentSystem->CanStartReload())
    {
        const int32 Sequence = EquipmentState.NextActionSequence++;
        const float DurationSeconds = EquipmentSystem->GetReloadDuration();
        ActionState.Begin(
            EBBBCharacterActionType::Reload,
            WorldTimeSeconds,
            DurationSeconds,
            Sequence,
            EquipmentSystem->GetMagazineRemoveNormalizedTime(),
            EquipmentSystem->GetMagazineSpawnNormalizedTime());

        FBBBEquipmentActionEvent ActionEvent;
        ActionEvent.ActionType = EBBBCharacterActionType::Reload;
        ActionEvent.EquipmentId = GetEquipmentId(*ActiveInstance);
        ActionEvent.Sequence = Sequence;
        ActionEvent.DurationSeconds = DurationSeconds;
        EquipmentEvents.AddAction(MoveTemp(ActionEvent));
    }
}
