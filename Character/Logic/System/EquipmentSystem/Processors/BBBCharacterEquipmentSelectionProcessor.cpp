#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(FBBBCharacterEquipmentUpdateContext &Context) const
{
    auto &Selection = Context.SelectionState;
    auto &Inventory = Context.InventoryState;
    const bool bCreateRequested = Selection.bHasEquipmentRequest;
    Context.bHasSelectionResult = false;
    Context.PendingEquipmentClass = nullptr;
    Context.bCreateRequested = false;

    if (bCreateRequested)
    {
        Selection.bHasEquipmentRequest = false;
        Selection.PendingSlot.Reset();
        if (!Selection.PendingEquipmentId.IsNone())
        {
            const UBBBEquipmentCatalog *Catalog = Context.Character.GetCharacterConfig().Equipment.EquipmentCatalog;
            const TSubclassOf<ABBBEquipment> Class = Catalog
                ? Catalog->FindEquipmentClass(Selection.PendingEquipmentId)
                : nullptr;
            if (!ensureMsgf(Class, TEXT("无法找到装备定义 %s"), *Selection.PendingEquipmentId.ToString()))
            {
                return;
            }

            bool bHasCapacity = false;
            for (const FBBBCharacterItem &Item : Inventory.BackpackSlots)
            {
                if (!IsValid(Item.ItemActor.Get()) || Item.ItemActor == Selection.ActiveMainHandInstance)
                {
                    bHasCapacity = true;
                    break;
                }
            }

            if (!ensureMsgf(bHasCapacity, TEXT("装备容器已满 无法创建 %s"), *Selection.PendingEquipmentId.ToString()))
            {
                return;
            }

            Context.PendingEquipmentClass = Class;
        }
        else
        {
            Selection.DesiredMainHandInstance = nullptr;
        }

        Context.bCreateRequested = true;
    }

    if (Selection.PendingSlot.IsSet())
    {
        const int32 Slot = Selection.PendingSlot.GetValue();
        Selection.PendingSlot.Reset();
        Selection.DesiredMainHandInstance = Inventory.ItemBarSlots.IsValidIndex(Slot)
            ? Cast<ABBBEquipment>(Inventory.ItemBarSlots[Slot].ItemActor.Get())
            : nullptr;
    }

    // 已销毁对象不能继续被视为有效持有关系
    if (!IsValid(Selection.DesiredMainHandInstance))
    {
        Selection.DesiredMainHandInstance = nullptr;
    }

    Context.bHasSelectionResult = true;
}
