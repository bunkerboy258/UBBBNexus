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

    // 只把属于当前装备的恢复事实交给装备接口
    FBBBEquipmentExternalAPI &API = Equipment->GetExternalAPI();
    for (const FBBBEquipmentActionFact &Fact : Facts)
    {
        if (Fact.EquipmentId == Equipment->GetEquipmentId())
        {
            API.SubmitFact(Fact);
        }
    }

    // 将换弹动画阶段序号转发为装备接口的对应操作
    for (const int32 Sequence : Commands.DetachMagazineSequences)
    {
        API.SubmitDetachMagazine(Sequence);
    }

    for (const int32 Sequence : Commands.LoadMagazineSequences)
    {
        API.SubmitLoadMagazine(Sequence);
    }

    for (const int32 Sequence : Commands.CancelReloadSequences)
    {
        API.SubmitCancelReload(Sequence);
    }

    // 使用递增序号提交本帧开火和换弹动作
    if (bFire)
    {
        API.SubmitFire(State.NextActionSequence++);
    }
    if (bReload)
    {
        API.SubmitReload(State.NextActionSequence++);
    }
}
