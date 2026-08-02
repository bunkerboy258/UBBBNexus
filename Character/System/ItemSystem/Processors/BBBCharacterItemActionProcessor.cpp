
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterItemActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/BBBWeaponActor.h"

void FBBBCharacterItemActionProcessor::Update(
    FBBBCharacterItemCommands &ItemCommands,
    const FBBBCharacterEquipmentState &EquipmentState) const
{
    ABBBEquipmentActor *EquippedItemActor = EquipmentState.GetEquippedItemActor();
    if (!EquippedItemActor)
    {
        return;
    }
    ABBBWeaponActor *WeaponActor = Cast<ABBBWeaponActor>(EquippedItemActor);

    if (ItemCommands.ConsumeFire())
    {
        if (!ensureMsgf(WeaponActor, TEXT("[UBBBC]Fire command failed because equipped item is not a weapon")))
        {
            return;
        }

        WeaponActor->Fire();
    }

    if (ItemCommands.ConsumeReload())
    {

        if (!ensureMsgf(WeaponActor, TEXT("[UBBBC]Reload command failed because equipped item is not a weapon")))
        {
            return;
        }

        WeaponActor->Reload();
    }
}
