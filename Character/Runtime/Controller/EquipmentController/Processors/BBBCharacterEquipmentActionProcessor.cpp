#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBCharacterEquipmentActionProcessor::Update(
    FBBBCharacterEquipmentCommands &Commands, FBBBCharacterEquipmentState &State) const
{
    // 先取得当前主手装备并消费本帧恢复和动作命令
    ABBBEquipment *Equipment = State.GetActiveMainHandInstance();
    TArray<FBBBEquipmentActionFact> Facts = Commands.ConsumeFacts();
    const bool bFire = Commands.ConsumeFire();
    const bool bReload = Commands.ConsumeReload();
    if (!Equipment)
    {
        // 未装备时仍然消费命令但不向空装备提交动作
        return;
    }

    for (const FBBBEquipmentActionFact &Fact : Facts)
    {
        if (Fact.EquipmentId == Equipment->GetEquipmentId())
        {
            Equipment->SubmitCommand(FBBBEquipmentCommand{
                EBBBEquipmentCommandType::Fact,
                Fact.Sequence,
                Fact});
        }
    }

    // 使用递增序号提交本帧开火和换弹动作
    if (bFire)
    {
        Equipment->SubmitCommand(FBBBEquipmentCommand{EBBBEquipmentCommandType::Fire, State.NextActionSequence++});
    }
    if (bReload)
    {
        Equipment->SubmitCommand(FBBBEquipmentCommand{EBBBEquipmentCommandType::Reload, State.NextActionSequence++});
    }
}
