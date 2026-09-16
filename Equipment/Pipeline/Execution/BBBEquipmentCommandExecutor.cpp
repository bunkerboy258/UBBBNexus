#include "BBBWork/UBBBNexus/Equipment/Pipeline/Execution/BBBEquipmentCommandExecutor.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"

void FBBBEquipmentCommandExecutor::Execute(ABBBEquipmentInstance &Instance, const FBBBEquipmentCommand &Command)
{
    FBBBEquipmentActionEvent Event;
    Event.EquipmentId = Instance.GetEquipmentId();
    Event.Sequence = Command.Sequence;
    Event.ActionType = EBBBCharacterActionType::Reload;

    switch (Command.Type)
    {
    case EBBBEquipmentCommandType::Equip:
        if (!Instance.EquipSystem.SubmitEquipMontage(Instance, Command.Sequence))
        {
            return;
        }
        Event.ActionType = EBBBCharacterActionType::Equip;
        break;
    case EBBBEquipmentCommandType::Fire:
        if (!Instance.FireSystem.Fire(Instance, Command.Sequence))
        {
            return;
        }
        Event.ActionType = EBBBCharacterActionType::Fire;
        break;
    case EBBBEquipmentCommandType::Reload:
        if (!Instance.ReloadSystem.Begin(Instance, Command.Sequence))
        {
            return;
        }
        Event.Phase = EBBBCharacterEquipmentPhase::ReloadStarted;
        break;
    case EBBBEquipmentCommandType::DetachMagazine:
        Instance.ReloadSystem.DetachMagazine(Instance.RuntimeData);
        Event.Phase = EBBBCharacterEquipmentPhase::MagazineDetached;
        break;
    case EBBBEquipmentCommandType::LoadMagazine:
        Instance.ReloadSystem.LoadMagazine(Instance.RuntimeData, Instance.Definition->AmmoConfig);
        Event.Phase = EBBBCharacterEquipmentPhase::MagazineLoaded;
        break;
    case EBBBEquipmentCommandType::CancelReload:
        Instance.ReloadSystem.Cancel(Instance.RuntimeData);
        Event.Phase = EBBBCharacterEquipmentPhase::ReloadCancelled;
        break;
    }

    Event.LoadedAmmo = Instance.RuntimeData.Ammo.LoadedAmmo;
    if (ensureMsgf(Instance.CharacterAPI, TEXT("[UBBBE]Equipment result sink is unavailable")))
    {
        Instance.CharacterAPI->PublishEquipmentEvent(Event);
    }
}

void FBBBEquipmentCommandExecutor::ApplySnapshot(ABBBEquipmentInstance &Instance, const FBBBEquipmentActionEvent &Snapshot)
{
    //镜像直接接受事实，表现依赖失效也不阻止事实还原
    Instance.RuntimeData.Ammo.LoadedAmmo = Snapshot.LoadedAmmo;
    if (Snapshot.ActionType == EBBBCharacterActionType::Equip)
    {
        Instance.EquipSystem.SubmitEquipMontage(Instance, Snapshot.Sequence);
    }
    if (Snapshot.ActionType == EBBBCharacterActionType::Fire)
    {
        Instance.FireSystem.Fire(Instance, Snapshot.Sequence);
    }
    if (Snapshot.Phase == EBBBCharacterEquipmentPhase::ReloadStarted)
    {
        Instance.ReloadSystem.Begin(Instance, Snapshot.Sequence);
        Instance.RuntimeData.Reload.bIsReloading = true;
        Instance.RuntimeData.Reload.bMagazineDetached = false;
        Instance.RuntimeData.Reload.Sequence = Snapshot.Sequence;
    }
    if (Snapshot.Phase == EBBBCharacterEquipmentPhase::MagazineDetached)
    {
        Instance.RuntimeData.Reload.bMagazineDetached = true;
    }
    if (Snapshot.Phase == EBBBCharacterEquipmentPhase::MagazineLoaded
        || Snapshot.Phase == EBBBCharacterEquipmentPhase::ReloadCancelled)
    {
        Instance.RuntimeData.Reload.bIsReloading = false;
    }

    if (Instance.CharacterAPI)
    {
        Instance.CharacterAPI->PublishEquipmentEvent(Snapshot);
    }
}
