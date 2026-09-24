#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(FBBBCharacterEquipmentUpdateContext &Context) const
{
    auto &Selection = Context.SelectionState;
    auto &Inventory = Context.InventoryState;
    const bool bCreateRequested = Selection.bHasEquipmentRequest;
    if (bCreateRequested)
    {
        Selection.bHasEquipmentRequest = false;
        Selection.PendingSlot.Reset();
        ABBBEquipment *Created = nullptr;
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

            Created = FBBBCharacterEquipmentLifecycleProcessor::Create(Context.Character, Class);
            if (!Created)
            {
                return;
            }
        }

        Selection.DesiredMainHandInstance = Created;
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

    if (Selection.ActiveMainHandInstance == Selection.DesiredMainHandInstance
        && IsValid(Selection.ActiveMainHandInstance))
    {
        return;
    }

    ABBBEquipment *Previous = Selection.ActiveMainHandInstance;
    if (IsValid(Previous))
    {
        Previous->OnUnequipped();
        if (Context.bIsMirror || bCreateRequested)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(&Context.CharacterMesh, *Previous);
        }
        if (!Context.bIsMirror && !bCreateRequested)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Detach(&Context.CharacterMesh, *Previous);
        }
    }

    for (FBBBCharacterItem &Item : Inventory.BackpackSlots)
    {
        if (!IsValid(Item.ItemActor.Get()))
        {
            Item.ItemActor = nullptr;
        }
    }

    for (FBBBCharacterItem &Item : Inventory.ItemBarSlots)
    {
        if (!IsValid(Item.ItemActor.Get()))
        {
            Item.ItemActor = nullptr;
        }
    }

    ABBBEquipment *Desired = Selection.DesiredMainHandInstance;
    Selection.ActiveMainHandInstance = nullptr;
    Selection.ActiveEquipmentId = NAME_None;
    if (!Desired)
    {
        return;
    }

    if (!ensureMsgf(FBBBCharacterEquipmentLifecycleProcessor::Attach(
        Context.CharacterMesh, Context.RightHandWeaponSocketName, *Desired),
        TEXT("装备挂接失败 %s"), *GetNameSafe(Desired)))
    {
        FBBBCharacterEquipmentLifecycleProcessor::Destroy(&Context.CharacterMesh, *Desired);
        Selection.DesiredMainHandInstance = nullptr;
        return;
    }

    Selection.ActiveMainHandInstance = Desired;
    Selection.ActiveEquipmentId = Desired->GetEquipmentId();
    if (bCreateRequested)
    {
        for (FBBBCharacterItem &Item : Inventory.BackpackSlots)
        {
            if (!Item.ItemActor)
            {
                Item.ItemActor = Desired;
                break;
            }
        }

        for (FBBBCharacterItem &Item : Inventory.ItemBarSlots)
        {
            if (!Item.ItemActor)
            {
                Item.ItemActor = Desired;
                break;
            }
        }
    }

    Desired->SubmitEquipInput();
}
