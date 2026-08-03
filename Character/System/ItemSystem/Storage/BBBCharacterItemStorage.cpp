#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Storage/BBBCharacterItemStorage.h"

#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/States/BBBCharacterItemStates.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"

bool FBBBCharacterItemStorage::AddItem(
    FBBBCharacterBackpackState &Backpack,
    UBBBItemInstance &ItemInstance) const
{
    if (!ensureMsgf(ItemInstance.IsValid(), TEXT("[UBBBC]Cannot add an invalid item instance to backpack")))
    {
        return false;
    }

    for (TObjectPtr<UBBBItemInstance> &Slot : Backpack.Slots)
    {
        if (!Slot)
        {
            Slot = &ItemInstance;
            return true;
        }
    }

    return false;
}

bool FBBBCharacterItemStorage::BindQuickAccessItem(
    FBBBCharacterBackpackState &Backpack,
    int32 QuickAccessSlot,
    UBBBItemInstance &ItemInstance) const
{
    if (!ensureMsgf(Backpack.QuickAccessBindings.IsValidIndex(QuickAccessSlot), TEXT("[UBBBC]Quick access slot is out of range")))
    {
        return false;
    }

    if (!ensureMsgf(Backpack.Slots.Contains(&ItemInstance), TEXT("[UBBBC]Quick access item does not belong to backpack")))
    {
        return false;
    }

    Backpack.QuickAccessBindings[QuickAccessSlot] = &ItemInstance;
    return true;
}
