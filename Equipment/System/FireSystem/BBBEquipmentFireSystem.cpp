#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/BBBEquipmentFireSystem.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Definition/BBBEquipmentFragmentContexts.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

bool FBBBEquipmentFireSystem::Fire(
    ABBBEquipmentInstance &Instance,
    const int32 Sequence) const
{
    UWorld *World = Instance.GetWorld();
    USkeletalMeshComponent *WeaponMesh = Instance.EquipmentSkeletalMesh;
    const UBBBEquipmentDefinition *Definition = Instance.Definition;
    if (!ensureMsgf(Instance.bIsActive && World && WeaponMesh && Definition && Instance.CharacterAPI
        && Definition->FireFragment.IsValid(), TEXT("[UBBBE]Equipment fire dependencies are invalid")))
    {
        return false;
    }

    FBBBEquipmentFireContext Context{
        Instance, *World, *WeaponMesh, Instance.RuntimeData, *Instance.CharacterAPI, Sequence, Instance.bIsMirror};
    if (!Definition->FireFragment.Get().Fire(Context))
    {
        return false;
    }

    Instance.AnimationSystem.RecordFire();
    return true;
}
