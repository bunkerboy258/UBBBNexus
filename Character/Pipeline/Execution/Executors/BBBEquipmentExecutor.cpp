
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentExecutor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"

void FBBBEquipmentExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    const FBBBCharacterItemInventoryState &InventoryState,
    FBBBCharacterEquipmentState &EquipmentState) const
{
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForExecution(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Approved)
        { continue; }

        if (Request.GetType() == EBBBCharacterActionType::Equip)
        {
            Request.MarkConsumed();

            FBBBItemInstance ItemInstance;

            //快捷栏为空 或映射失效时停止本次切换
            if (!GetHotbarItem(InventoryState, Request.GetEquipSlot(), ItemInstance))
            {
                UE_LOG(LogTemp, Warning, TEXT("Requested hotbar item does not exist"));
                continue;
            }

            if (!ItemInstance.Definition
                //验证物品定义合法性
                || !Cast<UBBBEquipmentDefinition>(ItemInstance.Definition))
            {
                UE_LOG(LogTemp, Warning, TEXT("Requested hotbar item cannot be equipped"));

                continue;
            }

            //重复选择时不响应
            if (EquipmentState.DesiredMainHandItem.InstanceId == ItemInstance.InstanceId)
            { continue; }

            //写入期望物品
            EquipmentState.TargetMode = EBBBEquipmentTargetMode::RuntimeItem;
            EquipmentState.DesiredMirrorHandle = NAME_None;
            EquipmentState.DesiredMainHandItem = ItemInstance;

        }
    }
}

//检查请求装备的目标物品是否合法
bool FBBBEquipmentExecutor::GetHotbarItem(
    const FBBBCharacterItemInventoryState &InventoryState,
    int32 HotbarSlot,
    FBBBItemInstance &OutItem)
{
    //验证索引没有越界
    if (HotbarSlot < 0 || HotbarSlot >= InventoryState.HotbarItemInstanceIds.Num())
    {
        return false;
    }

    const FGuid &ItemInstanceId = InventoryState.HotbarItemInstanceIds[HotbarSlot];

    if (!ItemInstanceId.IsValid())
    {
        return false;
    }

    //线性查找物品(*未来如果有高频/大容量查找需求 需要改进算法)
    for (const FBBBInventoryEntry &Entry : InventoryState.MainInventory.Entries)
    {
        if (Entry.ItemInstance.InstanceId == ItemInstanceId)
        {
            OutItem = Entry.ItemInstance;

            return true;
        }
    }

    return false;
}
