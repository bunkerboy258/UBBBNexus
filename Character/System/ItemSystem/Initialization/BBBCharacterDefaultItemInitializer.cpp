#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Initialization/BBBCharacterDefaultItemInitializer.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Storage/BBBCharacterItemStorage.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

void FBBBCharacterDefaultItemInitializer::Initialize(
    FBBBCharacterItemRuntimeData &ItemData,
    FBBBCharacterItemStorage &Storage,
    UObject &ItemOuter,
    const FBBBCharacterItemConfig &InItemConfig) const
{
    FBBBCharacterItemState &ItemState = ItemData.State;

    for (const FBBBDefaultInventoryItem &DefaultItem : InItemConfig.DefaultItems)
    {
        UBBBEquipmentDefinition *Definition = Cast<UBBBEquipmentDefinition>(DefaultItem.Definition);
        if (!ensureMsgf(Definition, TEXT("[UBBBC]Default item definition is not equipment")))
        { continue; }

        UBBBEquipmentInstance *NewInstance = UBBBEquipmentInstance::Create(
            ItemOuter,
            *Definition,
            DefaultItem.StackCount);

        if (!NewInstance)
        { continue; }

        if (!Storage.AddItem(ItemState, *NewInstance))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to add default item to inventory"));
            continue;
        }

        if (DefaultItem.HotbarSlot != INDEX_NONE
            && !Storage.AssignHotbarItem(ItemState, DefaultItem.HotbarSlot, NewInstance->GetInstanceId()))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to assign default item to hotbar"));
        }
    }

    if (!InItemConfig.bAutoEquipFirstHotbarItem)
    { return; }

    UBBBEquipmentInstance *FirstHotbarItem = Cast<UBBBEquipmentInstance>(Storage.GetHotbarItem(ItemState, 0));
    if (!FirstHotbarItem)
    { return; }

    ItemState.Equipment.DesiredMainHandInstance = FirstHotbarItem;
}
