#include "BBBWork/UBBBNexus/Equipment/Instance/ExternalAPI/BBBEquipmentExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInputRuntimeData.h"

void FBBBEquipmentExternalAPI::Initialize(FBBBEquipmentInputRuntimeData &InInput, const bool bInIsMirror)
{
    Input = &InInput;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentExternalAPI::Enqueue(const EBBBEquipmentInputType Type, const int32 Sequence)
{
    // 外部输入必须在游戏线程提交有效序号
    if (!ensureMsgf(IsInGameThread() && Input
        && (Sequence > 0 || Type == EBBBEquipmentInputType::CancelPendingActions),
        TEXT("[UBBBE]Invalid equipment input")))
    {
        return;
    }

    if (bIsMirror)
    {
        // 镜像装备只接收网络恢复输入
        return;
    }

    // 将输入封装后放入装备待处理队列
    FBBBEquipmentInput Entry;
    Entry.Type = Type;
    Entry.Sequence = Sequence;
    Input->Pending.Add(MoveTemp(Entry));
}

void FBBBEquipmentExternalAPI::SubmitEquip(const int32 Sequence)
{
    Enqueue(EBBBEquipmentInputType::Equip, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitFire(const int32 Sequence)
{
    Enqueue(EBBBEquipmentInputType::Fire, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitReload(const int32 Sequence)
{
    Enqueue(EBBBEquipmentInputType::Reload, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitDetachMagazine(const int32 Sequence)
{
    Enqueue(EBBBEquipmentInputType::DetachMagazine, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitLoadMagazine(const int32 Sequence)
{
    Enqueue(EBBBEquipmentInputType::LoadMagazine, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitCancelReload(const int32 Sequence)
{
    Enqueue(EBBBEquipmentInputType::CancelReload, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitCancelPendingActions()
{
    Enqueue(EBBBEquipmentInputType::CancelPendingActions, INDEX_NONE);
}

void FBBBEquipmentExternalAPI::ApplySnapshot(const FBBBEquipmentActionFact &Snapshot)
{
    // 镜像装备只允许接收恢复快照
    if (!ensureMsgf(IsInGameThread() && Input && bIsMirror, TEXT("[UBBBE]Snapshot input requires a mirror instance")))
    {
        return;
    }

    // 将恢复快照放入待处理队列
    FBBBEquipmentInput Entry;
    Entry.Type = EBBBEquipmentInputType::Snapshot;
    Entry.Sequence = Snapshot.Sequence;
    Entry.Snapshot = Snapshot;
    Input->Pending.Add(MoveTemp(Entry));
}
