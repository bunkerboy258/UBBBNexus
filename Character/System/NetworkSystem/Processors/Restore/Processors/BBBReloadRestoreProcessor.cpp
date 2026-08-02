
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBReloadRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/BBBWeaponActor.h"

void FBBBReloadRestoreProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBReloadNetworkPacket &) const
{

    ABBBWeaponActor *WeaponActor = Cast<ABBBWeaponActor>(
        EquipmentState.GetEquippedItemActor());

    if (!ensureMsgf(WeaponActor, TEXT("[UBBBC]Reload presentation failed because remote weapon is missing")))
    {
        return;
    }

    WeaponActor->PresentReload();
}
