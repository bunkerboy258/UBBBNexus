#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Fragment/BBBMagazineReloadFragment.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Definition/BBBEquipmentReloadContext.h"

bool FBBBMagazineReloadFragment::CanReload(
    const int32 LoadedAmmo,
    const int32 AmmoCapacity) const
{
    return Montage && LoadedAmmo < AmmoCapacity;
}

bool FBBBMagazineReloadFragment::Begin(FBBBEquipmentReloadContext &Context) const
{
    if (!Montage || !Context.CharacterAPI.SubmitEquipmentMontage(Montage, 1.0f, Context.Sequence, true))
    {
        return false;
    }

    if (Context.bIsMirror)
    {
        return true;
    }

    Context.bIsReloading = true;
    Context.bMagazineDetached = false;
    Context.ReloadSequence = Context.Sequence;
    return true;
}

bool FBBBMagazineReloadFragment::DetachMagazine(FBBBEquipmentReloadContext &Context) const
{
    Context.LoadedAmmo = 0;
    Context.bMagazineDetached = true;
    return true;
}

bool FBBBMagazineReloadFragment::LoadMagazine(FBBBEquipmentReloadContext &Context) const
{
    Context.LoadedAmmo = Context.AmmoCapacity;
    Context.bIsReloading = false;
    return true;
}

bool FBBBMagazineReloadFragment::Cancel(FBBBEquipmentReloadContext &Context) const
{
    Context.bIsReloading = false;
    return true;
}
