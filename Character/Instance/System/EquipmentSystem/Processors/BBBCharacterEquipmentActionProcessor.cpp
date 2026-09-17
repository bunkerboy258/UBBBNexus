#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
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
    for (const FBBBCharacterReloadAnimationInput &Input : Commands.ReloadInputs)
    {
        switch (Input.Phase)
        {
        case EBBBCharacterReloadAnimationPhase::Start:
            API.SubmitDetachMagazine(Input.Sequence);
            break;
        case EBBBCharacterReloadAnimationPhase::End:
            API.SubmitLoadMagazine(Input.Sequence);
            break;
        case EBBBCharacterReloadAnimationPhase::Interrupted:
            API.SubmitCancelReload(Input.Sequence);
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
