#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterEquipmentInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

bool FBBBCharacterEquipInput::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    if (!Data.Equipment.Inventory.QuickAccessBindings.IsValidIndex(EquipSlot))
    {
        return false;
    }

    ABBBEquipment *Target = Data.Equipment.Inventory.QuickAccessBindings[EquipSlot];
    return IsValid(Target) && Target != Data.Equipment.Equipment.GetActiveMainHandInstance();
}

void FBBBCharacterEquipInput::Apply(FBBBCharacterRuntimeData &Data) const
{
    Data.Operation.SelectedEquipment = Data.Equipment.Inventory.QuickAccessBindings[EquipSlot];
}

void FBBBCharacterEquipInput::FinishFrame(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterOperationState &Operation = Data.Operation;
    if (!Operation.SelectedEquipment)
    {
        return;
    }

    // 有效切换直接抢占换弹 空槽请求不触发取消
    Operation.CancelReloadSequence = Operation.ReloadSequence;
    Operation.LastCompletedReloadSequence = FMath::Max(
        Operation.LastCompletedReloadSequence, Operation.ReloadSequence);
    Operation.ReloadSequence = INDEX_NONE;
    Operation.ReloadEquipment.Reset();
    Data.Equipment.Equipment.DesiredMainHandInstance = Operation.SelectedEquipment;
}

bool FBBBCharacterReloadInput::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    return Data.Equipment.Equipment.GetActiveMainHandInstance()
        && !Data.Operation.SelectedEquipment
        && Data.Operation.ReloadSequence <= 0;
}

void FBBBCharacterReloadInput::Apply(FBBBCharacterRuntimeData &Data) const
{
    Data.Operation.bReload = true;
    Data.Equipment.Commands.SubmitReload();
}

void FBBBCharacterReloadInput::BeginFrame(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterOperationState &Operation = Data.Operation;
    Operation.CancelReloadSequence = INDEX_NONE;
    Operation.SelectedEquipment = nullptr;
    Operation.bFire = false;
    Operation.bReload = false;

    // 换装后的旧反馈不能使已经失效的换弹重新进入进行中状态
    if (Operation.ReloadSequence > 0
        && Operation.ReloadEquipment.Get() != Data.Equipment.Equipment.GetActiveMainHandInstance())
    {
        Operation.CancelReloadSequence = Operation.ReloadSequence;
        Operation.LastCompletedReloadSequence = Operation.ReloadSequence;
        Operation.ReloadSequence = INDEX_NONE;
        Operation.ReloadEquipment.Reset();
    }
}

bool FBBBCharacterFireInput::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    return AllowsFire(Data.Operation.ReloadSequence > 0,
        Data.Operation.SelectedEquipment != nullptr, Data.Operation.bReload);
}

bool FBBBCharacterFireInput::AllowsFire(const bool bReloading, const bool bSwitching,
    const bool bStartingReload)
{
    return !bReloading && !bSwitching && !bStartingReload;
}

void FBBBCharacterFireInput::Apply(FBBBCharacterRuntimeData &Data) const
{
    Data.Operation.bFire = true;
    Data.Operation.Control.bFire = true;
    Data.Equipment.Commands.SubmitFire();
}

void FBBBCharacterFireInput::ApplyHeld(FBBBCharacterRuntimeData &Data)
{
    const bool bHeldFire = Data.Operation.Control.bFire;
    Data.Operation.Control.bFire = false;
    FBBBCharacterFireInput Packet;
    if (bHeldFire && Packet.CanApply(Data))
    {
        Packet.Apply(Data);
    }
}

bool FBBBEquipmentActionEvent::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    return Sequence > 0;
}

void FBBBEquipmentActionEvent::Apply(FBBBCharacterRuntimeData &Data) const
{
    Data.Equipment.Events.AddAction(*this);
    if (!Data.Equipment.Equipment.GetActiveMainHandInstance()
        || EquipmentId != Data.Equipment.Equipment.GetActiveEquipmentId())
    {
        return;
    }

    FBBBCharacterOperationState &Operation = Data.Operation;
    if (Phase == EBBBCharacterEquipmentPhase::ReloadStarted
        && Sequence > Operation.LastCompletedReloadSequence
        && Sequence > Operation.ReloadSequence)
    {
        Operation.ReloadSequence = Sequence;
        Operation.ReloadEquipment = Data.Equipment.Equipment.GetActiveMainHandInstance();
        Operation.bMagazineDetached = false;
        Operation.bEndQueued = false;
    }
    if (Sequence != Operation.ReloadSequence)
    {
        return;
    }
    if (Phase == EBBBCharacterEquipmentPhase::MagazineDetached)
    {
        Operation.bMagazineDetached = true;
    }
    if (Phase == EBBBCharacterEquipmentPhase::MagazineLoaded
        || Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
    {
        if (Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
        {
            Operation.CancelReloadSequence = Sequence;
        }
        Operation.LastCompletedReloadSequence = Sequence;
        Operation.ReloadSequence = INDEX_NONE;
        Operation.ReloadEquipment.Reset();
    }
}
