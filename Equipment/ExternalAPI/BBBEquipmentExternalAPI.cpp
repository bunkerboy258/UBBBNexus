#include "BBBWork/UBBBNexus/Equipment/ExternalAPI/BBBEquipmentExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"

void FBBBEquipmentExternalAPI::Initialize(ABBBEquipmentInstance &InInstance)
{
    Instance = &InInstance;
}

void FBBBEquipmentExternalAPI::Enqueue(const EBBBEquipmentCommandType Type, const int32 Sequence)
{
    if (!ensureMsgf(IsInGameThread() && Instance && Sequence > 0, TEXT("[UBBBE]Invalid equipment command input")))
    {
        return;
    }

    if (Instance->bIsMirror)
    {
        return;
    }

    Instance->PendingCommands.Add({Type, Sequence});
}

void FBBBEquipmentExternalAPI::SubmitEquip(const int32 Sequence)
{
    Enqueue(EBBBEquipmentCommandType::Equip, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitFire(const int32 Sequence)
{
    Enqueue(EBBBEquipmentCommandType::Fire, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitReload(const int32 Sequence)
{
    Enqueue(EBBBEquipmentCommandType::Reload, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitDetachMagazine(const int32 Sequence)
{
    Enqueue(EBBBEquipmentCommandType::DetachMagazine, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitLoadMagazine(const int32 Sequence)
{
    Enqueue(EBBBEquipmentCommandType::LoadMagazine, Sequence);
}

void FBBBEquipmentExternalAPI::SubmitCancelReload(const int32 Sequence)
{
    Enqueue(EBBBEquipmentCommandType::CancelReload, Sequence);
}

void FBBBEquipmentExternalAPI::ApplySnapshot(const FBBBEquipmentActionEvent &Snapshot)
{
    if (!ensureMsgf(IsInGameThread() && Instance && Instance->bIsMirror, TEXT("[UBBBE]Snapshot input requires a mirror instance")))
    {
        return;
    }

    Instance->PendingSnapshots.Add(Snapshot);
}
