#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Initialization/BBBCharacterDefaultEquipmentInitializer.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"

void FBBBCharacterDefaultEquipmentInitializer::Initialize(
    FBBBCharacterEquipmentRuntimeData &EquipmentData,
    ABBBCharacterInstance &EquipmentHolder,
    const FBBBCharacterEquipmentConfig &EquipmentConfig) const
{
    FBBBCharacterEquipmentInventoryState &Inventory = EquipmentData.Inventory;

    for (const FBBBDefaultEquipment &DefaultEquipment : EquipmentConfig.DefaultEquipment)
    {
        if (!ensureMsgf(DefaultEquipment.Definition, TEXT("[UBBBC]Default equipment definition is null")))
        {
            continue;
        }

        ABBBEquipmentInstance *NewInstance = ABBBEquipmentInstance::Create(
            EquipmentHolder,
            *DefaultEquipment.Definition,
            !EquipmentHolder.IsLocallyControlled());

        if (!NewInstance)
        {
            continue;
        }

        USkeletalMeshComponent *CharacterMesh = EquipmentHolder.GetMesh();
        if (!ensureMsgf(CharacterMesh, TEXT("[UBBBC]Default equipment holder has no skeletal mesh"))
            || !NewInstance->BindHolder(*CharacterMesh, EquipmentConfig.RightHandWeaponSocketName))
        {
            NewInstance->Shutdown();
            continue;
        }

        TObjectPtr<ABBBEquipmentInstance> *EmptySlot = Inventory.Slots.FindByPredicate(
            [](const TObjectPtr<ABBBEquipmentInstance> &Slot)
            {
                return Slot == nullptr;
            });

        if (!ensureMsgf(EmptySlot, TEXT("[UBBBC]Equipment inventory has no empty slot")))
        {
            NewInstance->Shutdown();
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
