#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/BBBEquipmentReloadSystem.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"

bool FBBBEquipmentReloadSystem::Begin(ABBBEquipmentInstance &Instance, const int32 Sequence) const
{
    if (!ensureMsgf(Instance.Definition && Instance.CharacterAPI, TEXT("[UBBBE]Reload dependencies are invalid")))
    {
        return false;
    }

    if (!Instance.CharacterAPI->SubmitEquipmentMontage(Instance.Definition->ReloadConfig.Montage, 1.0f, Sequence, true))
    {
        return false;
    }

    Instance.RuntimeData.Reload.bIsReloading = true;
    Instance.RuntimeData.Reload.bMagazineDetached = false;
    Instance.RuntimeData.Reload.Sequence = Sequence;
    return true;
}


void FBBBEquipmentReloadSystem::DetachMagazine(FBBBEquipmentRuntimeData &Runtime) const
{
    Runtime.Ammo.LoadedAmmo = 0;
    Runtime.Reload.bMagazineDetached = true;
}

void FBBBEquipmentReloadSystem::LoadMagazine(FBBBEquipmentRuntimeData &Runtime, const FBBBEquipmentAmmoConfig &Config) const
{
    Runtime.Ammo.LoadedAmmo = Config.AmmoCapacity;
    Runtime.Reload.bIsReloading = false;
}

void FBBBEquipmentReloadSystem::Cancel(FBBBEquipmentRuntimeData &Runtime) const
{
    Runtime.Reload.bIsReloading = false;
}
