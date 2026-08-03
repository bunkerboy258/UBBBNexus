
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Initialization/BBBCharacterDefaultItemInitializer.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Storage/BBBCharacterItemStorage.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/RuntimeData/BBBItemRuntimeData.h"

void FBBBCharacterDefaultItemInitializer::Initialize(
    FBBBCharacterItemRuntimeData &ItemData,
    FBBBCharacterItemStorage &Storage,
    UObject &ItemOuter,
    const FBBBCharacterItemConfig &InItemConfig) const
{

    FBBBCharacterItemState &ItemState = ItemData.State;

    const FBBBCharacterItemConfig &Config = InItemConfig;
    for (const FBBBDefaultInventoryItem &DefaultItem : Config.DefaultItems)
    {
        if (!DefaultItem.Definition)
        {
            UE_LOG(LogTemp, Warning, TEXT("Skipped default item without definition"));
            continue;
        }

        FBBBItemInstance NewInstance;

        NewInstance.InstanceId = FGuid::NewGuid();

        NewInstance.Definition = DefaultItem.Definition;

        NewInstance.RuntimeData = NewObject<UBBBItemRuntimeData>(&ItemOuter);

        NewInstance.RuntimeData->Initialize(*DefaultItem.Definition);

        NewInstance.StackCount = FMath::Clamp(DefaultItem.StackCount, 1, DefaultItem.Definition->MaxStack);

        if (!Storage.AddItem(ItemState, NewInstance))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to add default item to inventory"));
            continue;
        }

        if (DefaultItem.HotbarSlot != INDEX_NONE && !Storage.AssignHotbarItem(ItemState, DefaultItem.HotbarSlot, NewInstance.InstanceId))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to assign default item to hotbar"));
        }
    }
    if (!Config.bAutoEquipFirstHotbarItem)
    {
        return;
    }
    FBBBItemInstance FirstHotbarItem;
    if (!Storage.GetHotbarItem(ItemState, 0, FirstHotbarItem))
    {
        return;
    }
    if (!FirstHotbarItem.Definition || !Cast<UBBBEquipmentDefinition>(FirstHotbarItem.Definition))
    {
        UE_LOG(LogTemp, Warning, TEXT("First hotbar item cannot be equipped"));
        return;
    }

    ItemState.Equipment.TargetMode = EBBBEquipmentTargetMode::RuntimeItem;
    ItemState.Equipment.DesiredMirrorHandle = NAME_None;
    ItemState.Equipment.DesiredMainHandItem = FirstHotbarItem;
}
