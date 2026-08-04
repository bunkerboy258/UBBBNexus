#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Initialization/BBBCharacterDefaultEquipmentInitializer.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

void FBBBCharacterDefaultEquipmentInitializer::Initialize(
    FBBBCharacterEquipmentRuntimeData &EquipmentData,
    UObject &EquipmentOuter,
    const FBBBCharacterEquipmentConfig &EquipmentConfig) const
{
    FBBBCharacterEquipmentInventoryState &Inventory = EquipmentData.Inventory;

    for (const FBBBDefaultEquipment &DefaultEquipment : EquipmentConfig.DefaultEquipment)
    {
        if (!ensureMsgf(DefaultEquipment.Definition, TEXT("[UBBBC]Default equipment definition is null")))
        {
            continue;
        }

        UBBBEquipmentInstance *NewInstance = UBBBEquipmentInstance::Create(
            EquipmentOuter,
            *DefaultEquipment.Definition);

        if (!NewInstance)
        {
            continue;
        }

        TObjectPtr<UBBBEquipmentInstance> *EmptySlot = Inventory.Slots.FindByPredicate(
            [](const TObjectPtr<UBBBEquipmentInstance> &Slot)
            {
                return Slot == nullptr;
            });

        if (!ensureMsgf(EmptySlot, TEXT("[UBBBC]Equipment inventory has no empty slot")))
        {
            continue;
        }

        *EmptySlot = NewInstance;

        if (DefaultEquipment.QuickAccessSlot == INDEX_NONE)
        {
            continue;
        }

        if (!ensureMsgf(
            Inventory.QuickAccessBindings.IsValidIndex(DefaultEquipment.QuickAccessSlot),
            TEXT("[UBBBC]Default equipment quick access slot is out of range")))
        {
            continue;
        }

        Inventory.QuickAccessBindings[DefaultEquipment.QuickAccessSlot] = NewInstance;
    }

    if (!EquipmentConfig.bAutoEquipFirstQuickAccessEquipment || Inventory.QuickAccessBindings.IsEmpty())
    {
        return;
    }

    EquipmentData.Equipment.DesiredMainHandInstance = Inventory.QuickAccessBindings[0];
}
