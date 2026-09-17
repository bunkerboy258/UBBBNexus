#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/BBBEquipmentReloadSystem.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Definition/BBBEquipmentFragmentContexts.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"

bool FBBBEquipmentReloadSystem::Begin(ABBBEquipmentInstance &Instance, const int32 Sequence) const
{
    if (!ensureMsgf(Instance.Definition && Instance.CharacterAPI && Instance.Definition->ReloadFragment.IsValid(),
        TEXT("[UBBBE]Reload dependencies are invalid")))
    {
        return false;
    }

    FBBBEquipmentReloadContext Context{
        Instance.RuntimeData, *Instance.CharacterAPI, Instance.Definition->AmmoConfig, Sequence};
    return Instance.Definition->ReloadFragment.Get().Begin(Context);
}

bool FBBBEquipmentReloadSystem::DetachMagazine(ABBBEquipmentInstance &Instance, const int32 Sequence) const
{
    if (!ensureMsgf(Instance.Definition && Instance.CharacterAPI && Instance.Definition->ReloadFragment.IsValid(),
        TEXT("[UBBBE]Reload detach dependencies are invalid")))
    {
        return false;
    }

    FBBBEquipmentReloadContext Context{
        Instance.RuntimeData, *Instance.CharacterAPI, Instance.Definition->AmmoConfig, Sequence};
    return Instance.Definition->ReloadFragment.Get().DetachMagazine(Context);
}

bool FBBBEquipmentReloadSystem::LoadMagazine(ABBBEquipmentInstance &Instance, const int32 Sequence) const
{
    if (!ensureMsgf(Instance.Definition && Instance.CharacterAPI && Instance.Definition->ReloadFragment.IsValid(),
        TEXT("[UBBBE]Reload load dependencies are invalid")))
    {
        return false;
    }

    FBBBEquipmentReloadContext Context{
        Instance.RuntimeData, *Instance.CharacterAPI, Instance.Definition->AmmoConfig, Sequence};
    return Instance.Definition->ReloadFragment.Get().LoadMagazine(Context);
}

bool FBBBEquipmentReloadSystem::Cancel(ABBBEquipmentInstance &Instance, const int32 Sequence) const
{
    if (!ensureMsgf(Instance.Definition && Instance.CharacterAPI && Instance.Definition->ReloadFragment.IsValid(),
        TEXT("[UBBBE]Reload cancel dependencies are invalid")))
    {
        return false;
    }

    FBBBEquipmentReloadContext Context{
        Instance.RuntimeData, *Instance.CharacterAPI, Instance.Definition->AmmoConfig, Sequence};
    return Instance.Definition->ReloadFragment.Get().Cancel(Context);
}
