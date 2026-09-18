#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBReloadBehavior.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBReloadBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return Data.Equipment.Equipment.GetActiveMainHandInstance()
        != nullptr;
}

void FBBBReloadBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.bReload = true;
    Data.Operation.ActiveBehaviorGroups |= BBBBehaviorGroup::Reload;
    Data.Equipment.Commands.SubmitReload();
}

void FBBBReloadBehavior::BeginFrame(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterOperationState &Operation = Data.Operation;
    Operation.CancelReloadSequence = INDEX_NONE;
    Operation.SelectedEquipment = nullptr;
    Operation.bFire = false;
    Operation.bReload = false;
    Operation.ActiveBehaviorGroups = 0;

    // 旧装备反馈不能复活已经失效的换弹
    if (Operation.ReloadSequence > 0
        && Operation.ReloadEquipment.Get() != Data.Equipment.Equipment.GetActiveMainHandInstance())
    {
        Cancel(Data);
    }
    if (Operation.ReloadSequence > 0)
    {
        Operation.ActiveBehaviorGroups |= BBBBehaviorGroup::Reload;
    }
}

void FBBBReloadBehavior::OnEquipmentResult(const FBBBEquipmentActionEvent &Event,
    FBBBCharacterRuntimeData &Data)
{
    if (!Data.Equipment.Equipment.GetActiveMainHandInstance()
        || Event.EquipmentId != Data.Equipment.Equipment.GetActiveEquipmentId())
    {
        return;
    }

    FBBBCharacterOperationState &Operation = Data.Operation;
    if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadStarted
        && Event.Sequence > Operation.LastCompletedReloadSequence
        && Event.Sequence > Operation.ReloadSequence)
    {
        Operation.ReloadSequence = Event.Sequence;
        Operation.ReloadEquipment = Data.Equipment.Equipment.GetActiveMainHandInstance();
        Operation.bMagazineDetached = false;
        Operation.bEndQueued = false;
        Operation.ActiveBehaviorGroups |= BBBBehaviorGroup::Reload;
    }
    if (Event.Sequence != Operation.ReloadSequence)
    {
        return;
    }
    if (Event.Phase == EBBBCharacterEquipmentPhase::MagazineDetached)
    {
        Operation.bMagazineDetached = true;
    }
    if (Event.Phase == EBBBCharacterEquipmentPhase::MagazineLoaded
        || Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
    {
        if (Event.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
        {
            Operation.CancelReloadSequence = Event.Sequence;
        }
        Operation.LastCompletedReloadSequence = Event.Sequence;
        Operation.ReloadSequence = INDEX_NONE;
        Operation.ReloadEquipment.Reset();
        Operation.ActiveBehaviorGroups &= ~BBBBehaviorGroup::Reload;
    }
}

void FBBBReloadBehavior::OnAnimationEvent(const FBBBCharacterReloadAnimationInput &Event,
    FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterOperationState &Operation = Data.Operation;
    if (Event.Sequence != Operation.ReloadSequence || Operation.ReloadSequence <= 0 || Operation.bEndQueued)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[UBBBC]Ignored stale animation input Sequence=%d"), Event.Sequence);
        return;
    }
    if (Event.Phase == EBBBCharacterReloadAnimationPhase::Start && Operation.bMagazineDetached)
    {
        return;
    }
    if (Event.Phase == EBBBCharacterReloadAnimationPhase::End && !Operation.bMagazineDetached)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload load arrived before detach Sequence=%d"), Event.Sequence);
        return;
    }

    if (Event.Phase == EBBBCharacterReloadAnimationPhase::Start)
    {
        Operation.bMagazineDetached = true;
    }
    else
    {
        Operation.bEndQueued = true;
        if (Event.Phase == EBBBCharacterReloadAnimationPhase::Interrupted)
        {
            Operation.CancelReloadSequence = Event.Sequence;
        }
    }
    Data.Equipment.Commands.ReloadInputs.Add(Event);
}

void FBBBReloadBehavior::Cancel(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterOperationState &Operation = Data.Operation;
    Operation.CancelReloadSequence = Operation.ReloadSequence;
    Operation.LastCompletedReloadSequence = FMath::Max(
        Operation.LastCompletedReloadSequence, Operation.ReloadSequence);
    Operation.ReloadSequence = INDEX_NONE;
    Operation.ReloadEquipment.Reset();
    Operation.ActiveBehaviorGroups &= ~BBBBehaviorGroup::Reload;
}
