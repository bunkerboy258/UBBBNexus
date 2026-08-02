
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBFireRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/BBBWeaponActor.h"

void FBBBFireRestoreProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBFireNetworkPacket &) const
{

    ABBBWeaponActor *WeaponActor = Cast<ABBBWeaponActor>(
        EquipmentState.GetEquippedItemActor());

    if (!ensureMsgf(WeaponActor, TEXT("[UBBBC]Fire presentation failed because remote weapon is missing")))
    {
        return;
    }

    WeaponActor->PresentFire();
}
