#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Execution/Executors/BBBEquipmentSelectionExecutor.h"

#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBEquipmentSelectionExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    const FBBBCharacterEquipmentInventoryState &InventoryState,
    FBBBCharacterEquipmentState &EquipmentState) const
{
    // 选择执行器只消费已经批准的装备请求
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForExecution(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Approved)
        {
            continue;
        }

        if (Request.GetType() == EBBBCharacterActionType::Equip)
        {
            // 请求进入执行阶段后标记为已消费避免重复选择
            Request.MarkConsumed();

            const int32 QuickAccessSlot = Request.GetEquipSlot();
            // 无效槽位无法解析目标装备
            if (!ensureMsgf(InventoryState.QuickAccessBindings.IsValidIndex(QuickAccessSlot), TEXT("[UBBBC]Requested quick access slot is out of range")))
            {
                continue;
            }

            // 空槽位不覆盖已有目标装备
            ABBBEquipment *EquipmentInstance = InventoryState.QuickAccessBindings[QuickAccessSlot];
            if (!EquipmentInstance)
            {
                continue;
            }

            // 重复选择当前目标时不产生额外写入
            if (EquipmentState.DesiredMainHandInstance == EquipmentInstance)
            {
                continue;
            }

            // 只写入目标装备实际切换由装备生命周期系统完成
            EquipmentState.DesiredMainHandInstance = EquipmentInstance;
        }
    }
}
