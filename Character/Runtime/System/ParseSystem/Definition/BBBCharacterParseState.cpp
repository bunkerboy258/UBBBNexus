#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Definition/BBBCharacterParseState.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBCharacterParseState::BeginFrame(const bool bInRestoreMode, ABBBEquipment *ActiveEquipment)
{
    bRestoreMode = bInRestoreMode;
    CancelReloadSequence = INDEX_NONE;
    SelectedEquipment = nullptr;
    bFire = false;
    bReload = false;

    // 瞬时意图每帧复位 基底包与请求包到来时重新登记
    Control.bFire = false;
    Control.bJump = false;

    // 追踪中的换弹装备失去主手资格时立即中止换弹
    if (ReloadSequence > 0 && ReloadEquipment.Get() != ActiveEquipment)
    {
        CancelReload();
    }
}

void FBBBCharacterParseState::SelectEquipment(ABBBEquipment &Equipment)
{
    SelectedEquipment = &Equipment;
}

void FBBBCharacterParseState::CancelReload()
{
    // 取消序号供蒙太奇守卫识别撤销的槽位期望 完成序号防止旧序号重复开启追踪
    CancelReloadSequence = ReloadSequence;
    LastCompletedReloadSequence = FMath::Max(LastCompletedReloadSequence, ReloadSequence);
    ReloadSequence = INDEX_NONE;
    ReloadEquipment.Reset();
    bMagazineDetached = false;
    bEndQueued = false;
}

void FBBBCharacterParseState::TrackReloadStarted(
    const int32 Sequence, const FName FactEquipmentId, ABBBEquipment *ActiveEquipment, const FName ActiveEquipmentId)
{
    // 事实不属于当前激活装备时只留档不驱动状态机
    if (!ActiveEquipment || FactEquipmentId != ActiveEquipmentId)
    {
        return;
    }

    // 过期或重复序号不重新开启追踪
    if (Sequence <= LastCompletedReloadSequence || Sequence <= ReloadSequence)
    {
        return;
    }

    ReloadSequence = Sequence;
    ReloadEquipment = ActiveEquipment;
    bMagazineDetached = false;
    bEndQueued = false;
}

void FBBBCharacterParseState::TrackMagazineDetached(
    const int32 Sequence, const FName FactEquipmentId, const FName ActiveEquipmentId)
{
    if (FactEquipmentId != ActiveEquipmentId || Sequence != ReloadSequence)
    {
        return;
    }

    bMagazineDetached = true;
}

void FBBBCharacterParseState::TrackReloadFinished(
    const int32 Sequence, const FName FactEquipmentId, const FName ActiveEquipmentId, const bool bCancelled)
{
    if (FactEquipmentId != ActiveEquipmentId || Sequence != ReloadSequence)
    {
        return;
    }

    // 取消完结要记录取消序号供蒙太奇守卫撤销对应槽位期望
    if (bCancelled)
    {
        CancelReloadSequence = Sequence;
    }

    LastCompletedReloadSequence = Sequence;
    ReloadSequence = INDEX_NONE;
    ReloadEquipment.Reset();
    bMagazineDetached = false;
    bEndQueued = false;
}

bool FBBBCharacterParseState::ReportMagazineDetached(const int32 Sequence)
{
    // 只接受当前换弹且尚未排队结束的通知
    if (!IsCurrentReloadSequence(Sequence) || bEndQueued)
    {
        return false;
    }

    // 重复卸下直接忽略
    if (bMagazineDetached)
    {
        return false;
    }

    bMagazineDetached = true;
    return true;
}

bool FBBBCharacterParseState::ReportMagazineLoaded(const int32 Sequence)
{
    if (!IsCurrentReloadSequence(Sequence) || bEndQueued)
    {
        return false;
    }

    // 装填先于卸下到达视为动画协议乱序
    if (!bMagazineDetached)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload load arrived before detach Sequence=%d"), Sequence);
        return false;
    }

    bEndQueued = true;
    return true;
}

bool FBBBCharacterParseState::ReportReloadInterrupted(const int32 Sequence)
{
    if (!IsCurrentReloadSequence(Sequence) || bEndQueued)
    {
        return false;
    }

    bEndQueued = true;
    CancelReloadSequence = Sequence;
    return true;
}
