#include "BBBWork/UBBBNexus/Equipment/Pipeline/Arbitration/BBBEquipmentActionArbitrator.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Definition/BBBEquipmentCommand.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"

bool FBBBEquipmentActionArbitrator::CanExecute(
    const FBBBEquipmentCommand &Command, const FBBBEquipmentRuntimeData &Runtime,
    const UBBBEquipmentDefinition &Definition, const float WorldTime, const bool bActive)
{
    if (!bActive || Command.Sequence <= 0)
    {
        return false;
    }

    switch (Command.Type)
    {
    case EBBBEquipmentCommandType::Equip:
        return true;
    case EBBBEquipmentCommandType::Fire:
        return !Runtime.Reload.bIsReloading && Runtime.Ammo.LoadedAmmo > 0
            && WorldTime - Runtime.Fire.LastFireTimeSeconds >= FMath::Max(Definition.FireConfig.FireInterval, 0.01f);
    case EBBBEquipmentCommandType::Reload:
        return !Runtime.Reload.bIsReloading && Definition.ReloadConfig.Montage
            && Runtime.Ammo.LoadedAmmo < Definition.AmmoConfig.AmmoCapacity;
    case EBBBEquipmentCommandType::DetachMagazine:
        return Runtime.Reload.bIsReloading && Runtime.Reload.Sequence == Command.Sequence
            && !Runtime.Reload.bMagazineDetached;
    case EBBBEquipmentCommandType::LoadMagazine:
        return Runtime.Reload.bIsReloading && Runtime.Reload.Sequence == Command.Sequence
            && Runtime.Reload.bMagazineDetached;
    case EBBBEquipmentCommandType::CancelReload:
        return Runtime.Reload.bIsReloading && Runtime.Reload.Sequence == Command.Sequence;
    }
    return false;
}
