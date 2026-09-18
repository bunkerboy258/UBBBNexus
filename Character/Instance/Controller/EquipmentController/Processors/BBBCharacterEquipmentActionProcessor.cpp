#include "BBBWork/UBBBNexus/Character/Instance/Controller/EquipmentController/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Controller/EquipmentController/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Instance/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBCharacterEquipmentActionProcessor::Update(
    FBBBCharacterEquipmentCommands &Commands, FBBBCharacterEquipmentState &State) const
{
    // 先取得当前主手装备并消费本帧恢复和动作命令
    ABBBEquipment *Equipment = State.GetActiveMainHandInstance();
    TArray<FBBBEquipmentActionEvent> Restored = Commands.ConsumeRestoredActions();
    const bool bFire = Commands.ConsumeFire();
    const bool bReload = Commands.ConsumeReload();
    if (!Equipment)
    {
        // 未装备时仍然消费命令但不向空装备提交动作
        return;
    }

    // 只把属于当前装备的恢复事实交给装备接口
    FBBBEquipmentExternalAPI &API = Equipment->GetExternalAPI();
    for (const FBBBEquipmentActionEvent &Event : Restored)
    {
        if (Event.EquipmentId == Equipment->GetEquipmentId())
        {
            API.ApplySnapshot(Event);
        }
    }

    // 将换弹动画通知映射为装备接口的对应阶段
    for (const FBBBCharacterDiscreteInput &Input : Commands.ReloadInputs)
    {
        switch (Input.ReloadPhase)
        {
        case EBBBCharacterReloadPhase::DetachMagazine:
            API.SubmitDetachMagazine(Input.Sequence);
            break;
        case EBBBCharacterReloadPhase::LoadMagazine:
            API.SubmitLoadMagazine(Input.Sequence);
            break;
        case EBBBCharacterReloadPhase::Interrupted:
            API.SubmitCancelReload(Input.Sequence);
            break;
        case EBBBCharacterReloadPhase::None:
            break;
        }
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
