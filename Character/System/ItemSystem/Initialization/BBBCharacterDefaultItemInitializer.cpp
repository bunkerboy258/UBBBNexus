#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Initialization/BBBCharacterDefaultItemInitializer.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Storage/BBBCharacterItemStorage.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentInstance.h"

void FBBBCharacterDefaultItemInitializer::Initialize(
    FBBBCharacterItemRuntimeData &ItemData,
    FBBBCharacterItemStorage &Storage,
    UObject &ItemOuter,
    const FBBBCharacterItemConfig &InItemConfig) const
{
    FBBBCharacterBackpackState &Backpack = ItemData.Backpack;

    for (const FBBBDefaultBackpackItem &DefaultItem : InItemConfig.DefaultBackpackItems)
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

        if (!Storage.AddItem(Backpack, *NewInstance))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to add default item to inventory"));
            continue;
        }

        if (DefaultItem.QuickAccessSlot != INDEX_NONE
            && !Storage.BindQuickAccessItem(Backpack, DefaultItem.QuickAccessSlot, *NewInstance))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to bind default item to quick access slot"));
        }
    }

    if (!InItemConfig.bAutoEquipFirstQuickAccessItem)
    { return; }

    if (Backpack.QuickAccessBindings.IsEmpty())
    { return; }

    UBBBEquipmentInstance *FirstQuickAccessItem = Cast<UBBBEquipmentInstance>(Backpack.QuickAccessBindings[0]);
    if (!FirstQuickAccessItem)
    { return; }

    ItemData.Equipment.DesiredMainHandInstance = FirstQuickAccessItem;
}
