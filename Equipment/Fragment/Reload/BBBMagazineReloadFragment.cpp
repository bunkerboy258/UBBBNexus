#include "BBBWork/UBBBNexus/Equipment/Fragment/Reload/BBBMagazineReloadFragment.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/Ammo/BBBEquipmentAmmoConfig.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Definition/BBBEquipmentFragmentContexts.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/BBBEquipmentRuntimeData.h"

bool FBBBMagazineReloadFragment::CanReload(
    const FBBBEquipmentRuntimeData &Runtime,
    const FBBBEquipmentAmmoConfig &AmmoConfig) const
{
    return Montage && Runtime.Ammo.LoadedAmmo < AmmoConfig.AmmoCapacity;
}

bool FBBBMagazineReloadFragment::Begin(FBBBEquipmentReloadContext &Context) const
{
    if (!Montage || !Context.CharacterAPI.SubmitEquipmentMontage(Montage, 1.0f, Context.Sequence, true))
    {
        return false;
    }

    Context.Runtime.Reload.bIsReloading = true;
    Context.Runtime.Reload.bMagazineDetached = false;
    Context.Runtime.Reload.Sequence = Context.Sequence;
    return true;
}

bool FBBBMagazineReloadFragment::DetachMagazine(FBBBEquipmentReloadContext &Context) const
{
    Context.Runtime.Ammo.LoadedAmmo = 0;
    Context.Runtime.Reload.bMagazineDetached = true;
    return true;
}

bool FBBBMagazineReloadFragment::LoadMagazine(FBBBEquipmentReloadContext &Context) const
{
    Context.Runtime.Ammo.LoadedAmmo = Context.AmmoConfig.AmmoCapacity;
    Context.Runtime.Reload.bIsReloading = false;
    return true;
}

bool FBBBMagazineReloadFragment::Cancel(FBBBEquipmentReloadContext &Context) const
{
    Context.Runtime.Reload.bIsReloading = false;
    return true;
}
