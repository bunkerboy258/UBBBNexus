#include "BBBWork/UBBBNexus/Equipment/Core/Update/BBBEquipmentUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Arbitration/BBBEquipmentActionArbitrator.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Execution/BBBEquipmentCommandExecutor.h"

void FBBBEquipmentUpdatePipeline::Update(
    ABBBEquipmentInstance &Instance,
    const float WorldTimeSeconds)
{
    if (!ensureMsgf(Instance.Definition, TEXT("[UBBBE]Equipment update has no definition")))
    {
        return;
    }

    TArray<FBBBEquipmentActionEvent> Snapshots = MoveTemp(Instance.PendingSnapshots);
    Instance.PendingSnapshots.Reset();
    for (const FBBBEquipmentActionEvent &Snapshot : Snapshots)
    {
        FBBBEquipmentCommandExecutor::ApplySnapshot(Instance, Snapshot);
    }

    TArray<FBBBEquipmentCommand> Commands = MoveTemp(Instance.PendingCommands);
    Instance.PendingCommands.Reset();
    for (const FBBBEquipmentCommand &Command : Commands)
    {
        if (FBBBEquipmentActionArbitrator::CanExecute(Command, Instance.RuntimeData,
            *Instance.Definition, WorldTimeSeconds, Instance.bIsActive))
        {
            FBBBEquipmentCommandExecutor::Execute(Instance, Command);
            continue;
        }

        if (Command.Type == EBBBEquipmentCommandType::DetachMagazine
            || Command.Type == EBBBEquipmentCommandType::LoadMagazine)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Rejected reload stage Type=%d Sequence=%d"),
                static_cast<int32>(Command.Type), Command.Sequence);
        }

        if (Command.Type == EBBBEquipmentCommandType::Reload && !Instance.Definition->ReloadConfig.Montage)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Reload rejected: no montage configured"));
        }
    }
}

void FBBBEquipmentUpdatePipeline::LateUpdate(
    ABBBEquipmentInstance &Instance,
    const float WorldTimeSeconds)
{
    if (!Instance.bIsActive)
    {
        return;
    }

    Instance.AnimationSystem.Update(Instance, WorldTimeSeconds);
}
