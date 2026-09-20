#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

void FBBBCharacterEquipmentActionProcessor::Update(
    FBBBCharacterEquipmentCommands &Commands,
    FBBBCharacterEquipmentState &State,
    const bool bIsMirror) const
{
    // 先取得当前主手装备并消费本帧恢复和动作命令
    ABBBEquipment *Equipment = State.GetActiveMainHandInstance();
    TArray<FBBBEquipmentActionFact> Facts = Commands.ConsumeFacts();
    const bool bFire = Commands.ConsumeFire();
    const bool bReload = Commands.ConsumeReload();
    bool bSecondaryActive = false;
    const bool bSecondarySubmitted = Commands.ConsumeSecondary(bSecondaryActive);
    if (!Equipment)
    {
        // 未装备时仍然消费命令但不向空装备提交动作
        return;
    }

    if (bIsMirror)
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

    if (bIsMirror)
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
