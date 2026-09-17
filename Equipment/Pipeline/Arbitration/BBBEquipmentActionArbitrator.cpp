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
        return !Runtime.Reload.bIsReloading && Definition.FireFragment.IsValid()
            && Definition.FireFragment.Get().CanFire(Runtime, WorldTime);
    case EBBBEquipmentCommandType::Reload:
        return !Runtime.Reload.bIsReloading && Definition.ReloadFragment.IsValid()
            && Definition.ReloadFragment.Get().CanReload(Runtime, Definition.AmmoConfig);
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
