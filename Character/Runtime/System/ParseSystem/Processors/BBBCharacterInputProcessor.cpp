#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

void FBBBCharacterInputProcessor::CancelReload(FBBBCharacterParseState &State)
{
    State.CancelReloadSequence = State.ReloadSequence;
    State.LastCompletedReloadSequence = FMath::Max(
        State.LastCompletedReloadSequence,
        State.ReloadSequence);
    State.ReloadSequence = INDEX_NONE;
    State.ReloadEquipment.Reset();
    State.bMagazineDetached = false;
    State.bEndQueued = false;
}

void FBBBCharacterInputProcessor::ApplyEquipmentEvent(
    const FBBBEquipmentActionEvent &Event, FBBBCharacterRuntimeData &Data)
{
    if (Event.ActionType == EBBBCharacterActionType::None || Event.Sequence <= 0)
    {
        return;
    }

    Data.Equipment.Events.AddAction(Event);
    FBBBCharacterParseState &State = Data.Operation;
    if (!Data.Equipment.Equipment.GetActiveMainHandInstance()
        || Event.EquipmentId != Data.Equipment.Equipment.GetActiveEquipmentId())
    {
        return;
    }

    if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadStarted
        && Event.Sequence > State.LastCompletedReloadSequence
        && Event.Sequence > State.ReloadSequence)
    {
        State.ReloadSequence = Event.Sequence;
        State.ReloadEquipment = Data.Equipment.Equipment.GetActiveMainHandInstance();
        State.bMagazineDetached = false;
        State.bEndQueued = false;
    }

    if (Event.Sequence != State.ReloadSequence)
    {
        return;
    }

    if (Event.Phase == EBBBCharacterEquipmentPhase::MagazineDetached)
    {
        State.bMagazineDetached = true;
    }

    if (Event.Phase == EBBBCharacterEquipmentPhase::MagazineLoaded
        || Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
    {
        if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
        {
            State.CancelReloadSequence = Event.Sequence;
        }
        State.LastCompletedReloadSequence = Event.Sequence;
        State.ReloadSequence = INDEX_NONE;
        State.ReloadEquipment.Reset();
        State.bMagazineDetached = false;
        State.bEndQueued = false;
    }
}

void FBBBCharacterInputProcessor::ApplyReloadPhase(
    const FBBBCharacterDiscreteInput &Input, FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterParseState &State = Data.Operation;
    if (Input.ReloadPhase == EBBBCharacterReloadPhase::None
        || Input.Sequence != State.ReloadSequence
        || State.ReloadSequence <= 0
        || State.bEndQueued)
    {
        return;
    }

    if (Input.ReloadPhase == EBBBCharacterReloadPhase::DetachMagazine && State.bMagazineDetached)
    {
        return;
    }

    if (Input.ReloadPhase == EBBBCharacterReloadPhase::LoadMagazine && !State.bMagazineDetached)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload load arrived before detach Sequence=%d"), Input.Sequence);
        return;
    }

    if (Input.ReloadPhase == EBBBCharacterReloadPhase::DetachMagazine)
    {
        State.bMagazineDetached = true;
    }

    if (Input.ReloadPhase == EBBBCharacterReloadPhase::LoadMagazine
        || Input.ReloadPhase == EBBBCharacterReloadPhase::Interrupted)
    {
        State.bEndQueued = true;
        if (Input.ReloadPhase == EBBBCharacterReloadPhase::Interrupted)
        {
            State.CancelReloadSequence = Input.Sequence;
        }
    }

    Data.Equipment.Commands.ReloadInputs.Add(Input);
}

void FBBBCharacterInputProcessor::PublishControl(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterParseState &State = Data.Operation;
    State.Control.bSprint = State.Control.bSprint && !State.Control.bAim && !State.Control.bFire;
    Data.Control.Value = State.Control;
}

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterRuntimeData &Data, UBBBEquipmentCatalog &Catalog, const bool bRestoreMode) const
{
    FBBBCharacterParseState &State = Data.Operation;
    State.bRestoreMode = bRestoreMode;
    State.CancelReloadSequence = INDEX_NONE;
    State.SelectedEquipment = nullptr;
    State.bFire = false;
    State.bReload = false;

    if (State.ReloadSequence > 0
        && State.ReloadEquipment.Get() != Data.Equipment.Equipment.GetActiveMainHandInstance())
    {
        CancelReload(State);
    }

    TArray<FBBBCharacterRestoreDiscreteInput> RestoreInputs = MoveTemp(Data.Input.PendingRestoreDiscrete);
    TArray<FBBBCharacterDiscreteInput> Inputs = MoveTemp(Data.Input.PendingDiscrete);

    FBBBCharacterMontagePacket::BeginFrame(Data);
    Data.CameraContributions.Reset();

    if (bRestoreMode)
    {
        for (const FBBBCharacterRestoreDiscreteInput &Input : RestoreInputs)
        {
            if (Input.EquipmentHandle != NAME_None)
            {
                UBBBEquipmentDefinition *Definition = Catalog.FindDefinition(Input.EquipmentHandle);
                if (ensureMsgf(Definition, TEXT("[UBBBC]Unknown equipment restore handle %s"),
                    *Input.EquipmentHandle.ToString()))
                {
                    Data.Equipment.Commands.PendingRestoredEquipment = Definition;
                }
            }

            if (Input.EquipmentEvent.ActionType != EBBBCharacterActionType::None)
            {
                if (Input.EquipmentEvent.ActionType == EBBBCharacterActionType::Fire
                    && State.ReloadSequence > 0)
                {
                    CancelReload(State);
                }
                Data.Equipment.Commands.SubmitRestoredAction(Input.EquipmentEvent);
                ApplyEquipmentEvent(Input.EquipmentEvent, Data);
            }

            if (Input.Aim.IsSet())
            {
                Data.Aim.ApplyRestoredState(Input.Aim.GetValue());
            }

            if (Input.Gait.IsSet())
            {
                Data.Locomotion.CommitGait(Input.Gait.GetValue());
            }
        }

        return;
    }

    const FBBBCharacterContinuousInput &Continuous = Data.Input.Continuous;
    State.Control.MoveWorld = Continuous.MoveWorld;
    State.Control.FacingWorld = Continuous.FacingWorld;
    State.Control.AimTargetWorld = Continuous.AimTargetWorld;
    State.Control.bAim = Continuous.bAim;
    State.Control.bWalk = Continuous.bWalk;
    State.Control.bSprint = Continuous.bSprint;
    State.Control.bCrouch = Continuous.bCrouch;
    State.Control.bFire = false;
    State.Control.bJump = false;

    for (const FBBBCharacterDiscreteInput &Input : Inputs)
    {
        ApplyEquipmentEvent(Input.EquipmentEvent, Data);
    }

    // 固定顺序表达动作优先级 后处理的效果可以清除前处理的事实
    for (const FBBBCharacterDiscreteInput &Input : Inputs)
    {
        State.bFire |= Input.bFire;
    }

    for (const FBBBCharacterDiscreteInput &Input : Inputs)
    {
        if (Input.bReload && Data.Equipment.Equipment.GetActiveMainHandInstance()
            && State.SelectedEquipment == nullptr && State.ReloadSequence <= 0)
        {
            State.bReload = true;
            State.bFire = false;
        }
    }

    for (const FBBBCharacterDiscreteInput &Input : Inputs)
    {
        if (!Data.Equipment.Inventory.QuickAccessBindings.IsValidIndex(Input.EquipSlot))
        {
            continue;
        }

        ABBBEquipment *Target = Data.Equipment.Inventory.QuickAccessBindings[Input.EquipSlot];
        if (!IsValid(Target) || Target == Data.Equipment.Equipment.GetActiveMainHandInstance())
        {
            continue;
        }

        if (State.ReloadSequence > 0)
        {
            CancelReload(State);
        }
        State.SelectedEquipment = Target;
        State.bReload = false;
        State.bFire = false;
        Data.Equipment.Equipment.DesiredMainHandInstance = Target;
    }

    for (const FBBBCharacterDiscreteInput &Input : Inputs)
    {
        State.Control.bJump |= Input.bJump;
        ApplyReloadPhase(Input, Data);
    }

    State.Control.bFire = State.bFire;
    PublishControl(Data);

    if (State.bFire)
    {
        Data.Equipment.Commands.SubmitFire();
    }

    if (State.bReload)
    {
        Data.Equipment.Commands.SubmitReload();
    }

    for (const FBBBCharacterDiscreteInput &Input : Inputs)
    {
        if (Input.Montage)
        {
            FBBBCharacterMontagePacket Montage;
            Montage.Montage = Input.Montage;
            Montage.PlayRate = Input.MontagePlayRate;
            Montage.Sequence = Input.Sequence;
            Montage.bReload = Input.bReloadMontage;
            if (Montage.CanApply(Data))
            {
                Montage.Apply(Data);
            }
        }

        if (Input.CameraRecoverySpeed > 0.0f)
        {
            Data.CameraContributions.Add({Input.CameraImpulse, Input.CameraRecoverySpeed});
        }
    }
}
