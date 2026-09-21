#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentCommandState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

void FBBBCharacterEquipmentActionProcessor::Update(
    FBBBCharacterEquipmentUpdateContext &Context) const
{
    FBBBCharacterEquipmentCommandState &Commands = Context.CommandState;
    FBBBCharacterEquipmentSelectionState &State = Context.SelectionState;
    ABBBEquipment *Equipment = State.ActiveMainHandInstance;
    TArray<FBBBEquipmentActionFact> Facts = MoveTemp(Commands.PendingFacts);
    const bool bFire = Commands.bActivateFire;
    const bool bReload = Commands.bActivateReload;
    const bool bSecondarySubmitted = Commands.bSecondarySubmitted;
    const bool bSecondaryActive = Commands.bSecondaryActive;

    Commands.bActivateFire = false;
    Commands.bActivateReload = false;
    Commands.bSecondarySubmitted = false;

    if (!Equipment)
    {
        // 未装备时仍然消费命令但不向空装备提交动作
        return;
    }

    if (Context.bIsMirror)
    {
        for (const FBBBEquipmentActionFact &Fact : Facts)
        {
            if (Fact.EquipmentId == Equipment->GetEquipmentId())
            {
                Equipment->SubmitCommand(FBBBEquipmentCommand{
                    EBBBEquipmentCommandType::Fact,
                    Fact.Sequence,
                    false,
                    Fact},
                    true);
            }
        }
    }

    if (Context.bIsMirror)
    {
        return;
    }

    // 使用递增序号提交本帧开火和换弹动作
    if (bFire)
    {
        Equipment->SubmitCommand(
            FBBBEquipmentCommand{EBBBEquipmentCommandType::Primary, State.NextActionSequence++},
            false);
    }
    if (bReload)
    {
        Equipment->SubmitCommand(
            FBBBEquipmentCommand{EBBBEquipmentCommandType::Reload, State.NextActionSequence++},
            false);
    }

    if (bSecondarySubmitted)
    {
        Equipment->SubmitCommand(
            FBBBEquipmentCommand{
                EBBBEquipmentCommandType::Secondary,
                State.NextActionSequence++,
                bSecondaryActive},
            false);
    }
}
