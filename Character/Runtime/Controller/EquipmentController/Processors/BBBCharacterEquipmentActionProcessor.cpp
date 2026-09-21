#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Context/BBBCharacterEquipmentActionContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

void FBBBCharacterEquipmentActionProcessor::Update(
    FBBBCharacterEquipmentCommands &Commands,
    FBBBCharacterEquipmentState &State,
    const bool bIsMirror) const
{
    // 先取得当前主手装备并消费本帧恢复和动作命令
    FBBBCharacterEquipmentActionContext Context;
    Context.Equipment = State.ActiveMainHandInstance;
    Context.Facts = Commands.ConsumeFacts();
    Context.bFire = Commands.ConsumeFire();
    Context.bReload = Commands.ConsumeReload();
    Context.bSecondarySubmitted = Commands.ConsumeSecondary(Context.bSecondaryActive);
    if (!Context.Equipment)
    {
        // 未装备时仍然消费命令但不向空装备提交动作
        return;
    }

    if (bIsMirror)
    {
        for (const FBBBEquipmentActionFact &Fact : Context.Facts)
        {
            if (Fact.EquipmentId == Context.Equipment->GetEquipmentId())
            {
                Context.Equipment->SubmitCommand(FBBBEquipmentCommand{
                    EBBBEquipmentCommandType::Fact,
                    Fact.Sequence,
                    false,
                    Fact},
                    true);
            }
        }
    }

    if (bIsMirror)
    {
        return;
    }

    // 使用递增序号提交本帧开火和换弹动作
    if (Context.bFire)
    {
        Context.Equipment->SubmitCommand(
            FBBBEquipmentCommand{EBBBEquipmentCommandType::Primary, State.NextActionSequence++},
            false);
    }
    if (Context.bReload)
    {
        Context.Equipment->SubmitCommand(
            FBBBEquipmentCommand{EBBBEquipmentCommandType::Reload, State.NextActionSequence++},
            false);
    }

    if (Context.bSecondarySubmitted)
    {
        Context.Equipment->SubmitCommand(
            FBBBEquipmentCommand{
                EBBBEquipmentCommandType::Secondary,
                State.NextActionSequence++,
                Context.bSecondaryActive},
            false);
    }
}
