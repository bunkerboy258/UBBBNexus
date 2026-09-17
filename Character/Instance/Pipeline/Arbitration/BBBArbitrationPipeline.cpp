#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/Rules/BBBCharacterConflictRules.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBArbitrationPipeline::Update(const bool bRestoreMode) const
{
    if (!ensureMsgf(Data, TEXT("[UBBBC]Arbitration pipeline is uninitialized")))
    {
        return;
    }
    FBBBCharacterOperationState &Operation = Data->Operation;
    FBBBInputBatch &Frame = Data->Input.Frame;
    ABBBEquipment *Active = Data->Equipment.Equipment.GetActiveMainHandInstance();
    Operation.CancelReloadSequence = INDEX_NONE;
    Operation.SelectedEquipment = nullptr;
    Operation.bFire = false;
    Operation.bReload = false;
    Operation.bRestoreMode = bRestoreMode;
    Operation.Control = Data->Input.FrameControl;

    // 操作只关联具体装备和递增序号 旧装备反馈不能复活已经结束的换弹
    if (Operation.ReloadSequence > 0 && Operation.ReloadEquipment.Get() != Active)
    {
        Operation.CancelReloadSequence = Operation.ReloadSequence;
        Operation.LastCompletedReloadSequence = Operation.ReloadSequence;
        Operation.ReloadSequence = INDEX_NONE;
        Operation.ReloadEquipment.Reset();
    }
    for (const FBBBEquipmentActionEvent &Result : Frame.Results)
    {
        if (!Active || Result.EquipmentId != Data->Equipment.Equipment.GetActiveEquipmentId())
        {
            continue;
        }
        if (Result.Phase == EBBBCharacterEquipmentPhase::ReloadStarted
            && Result.Sequence > Operation.LastCompletedReloadSequence
            && Result.Sequence > Operation.ReloadSequence)
        {
            Operation.ReloadSequence = Result.Sequence;
            Operation.ReloadEquipment = Active;
            Operation.bMagazineDetached = false;
            Operation.bEndQueued = false;
        }
        if (Result.Sequence != Operation.ReloadSequence)
        {
            continue;
        }
        if (Result.Phase == EBBBCharacterEquipmentPhase::MagazineDetached)
        {
            Operation.bMagazineDetached = true;
        }
        if (Result.Phase == EBBBCharacterEquipmentPhase::MagazineLoaded
            || Result.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
        {
            if (Result.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
            {
                Operation.CancelReloadSequence = Result.Sequence;
            }
            Operation.LastCompletedReloadSequence = Result.Sequence;
            Operation.ReloadSequence = INDEX_NONE;
            Operation.ReloadEquipment.Reset();
        }
    }

    // 镜像只消费已完成结果 不从本地控制或动画通知发起新的玩法
    if (bRestoreMode)
    {
        Frame.Notifications.Reset();
        return;
    }

    // 先解析有效切换目标 再决定是否中断换弹 空槽不产生取消效果
    for (const FBBBCharacterEquipmentInput &Request : Frame.Equipment)
    {
        if (Request.ActionType == EBBBCharacterActionType::Equip
            && Data->Equipment.Inventory.QuickAccessBindings.IsValidIndex(Request.EquipSlot))
        {
            ABBBEquipment *Target = Data->Equipment.Inventory.QuickAccessBindings[Request.EquipSlot];
            if (IsValid(Target) && Target != Active)
            {
                Operation.SelectedEquipment = Target;
            }
        }
    }
    if (Operation.SelectedEquipment)
    {
        Operation.CancelReloadSequence = Operation.ReloadSequence;
        Operation.LastCompletedReloadSequence = FMath::Max(
            Operation.LastCompletedReloadSequence, Operation.ReloadSequence);
        Operation.ReloadSequence = INDEX_NONE;
        Operation.ReloadEquipment.Reset();
    }

    // 反馈优先于普通请求处理 同一帧的卸匣与装匣通知按原始到达顺序消费
    Frame.Notifications.RemoveAll([&Operation](const FBBBCharacterReloadAnimationInput &Input)
    {
        if (Input.Sequence != Operation.ReloadSequence || Operation.ReloadSequence <= 0 || Operation.bEndQueued)
        {
            UE_LOG(LogTemp, Verbose, TEXT("[UBBBC]Ignored stale animation input Sequence=%d"), Input.Sequence);
            return true;
        }
        if (Input.Phase == EBBBCharacterReloadAnimationPhase::Start)
        {
            if (Operation.bMagazineDetached)
            {
                return true;
            }
            Operation.bMagazineDetached = true;
            return false;
        }
        if (Input.Phase == EBBBCharacterReloadAnimationPhase::End && !Operation.bMagazineDetached)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload load arrived before detach Sequence=%d"), Input.Sequence);
            return true;
        }
        Operation.bEndQueued = true;
        if (Input.Phase == EBBBCharacterReloadAnimationPhase::Interrupted)
        {
            Operation.CancelReloadSequence = Input.Sequence;
        }
        return false;
    });

    for (const FBBBCharacterEquipmentInput &Request : Frame.Equipment)
    {
        if (Request.ActionType == EBBBCharacterActionType::Reload)
        {
            Operation.bReload = Active && !Operation.SelectedEquipment && Operation.ReloadSequence <= 0;
        }
        if (Request.ActionType == EBBBCharacterActionType::Fire)
        {
            Operation.bFire = true;
        }
    }
    Operation.bFire = (Operation.bFire || Operation.Control.bFire)
        && FBBBCharacterConflictRules::AllowsFire(Operation.ReloadSequence > 0,
            Operation.SelectedEquipment != nullptr, Operation.bReload);
    Operation.Control.bFire = Operation.bFire;
    Operation.Control.bSprint = Operation.Control.bSprint
        && FBBBCharacterConflictRules::AllowsSprint(Operation.Control.bAim || Operation.Control.bFire);
}
