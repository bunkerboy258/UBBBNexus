#include "BBBWork/UBBBNexus/Equipment/Core/Initialization/BBBEquipmentInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBEquipmentInitializer::Initialize(ABBBEquipmentInstance &Instance)
{
    UBBBEquipmentDefinition *Definition = Instance.Definition;
    USkeletalMeshComponent *WeaponMesh = Instance.EquipmentSkeletalMesh;
    if (!ensureMsgf(Definition && WeaponMesh, TEXT("[UBBBE]Equipment initialization dependencies are invalid")))
    {
        return false;
    }

    if (!ensureMsgf(
        Definition->EquipmentMesh && Definition->EquipmentAnimationClass,
        TEXT("[UBBBE]Equipment '%s' needs mesh and animation class in its definition"),
        *Definition->EquipmentId.ToString()))
    {
        return false;
    }

    if (!ensureMsgf(
        Definition->EquipmentAnimationClass->IsChildOf(UBBBEquipmentAnimInstance::StaticClass()),
        TEXT("[UBBBE]Equipment animation class must derive from UBBBEquipmentAnimInstance")))
    {
        return false;
    }

    if (!ensureMsgf(Definition->EquipFragment.IsValid() && Definition->FireFragment.IsValid()
        && Definition->ReloadFragment.IsValid(),
        TEXT("[UBBBE]Equipment '%s' must configure equip, fire and reload fragments"),
        *Definition->EquipmentId.ToString()))
    {
        return false;
    }

    if (!ensureMsgf(Definition->EquipFragment.GetScriptStruct() != FBBBEquipmentEquipFragment::StaticStruct()
        && Definition->FireFragment.GetScriptStruct() != FBBBEquipmentFireFragment::StaticStruct()
        && Definition->ReloadFragment.GetScriptStruct() != FBBBEquipmentReloadFragment::StaticStruct(),
        TEXT("[UBBBE]Equipment '%s' must use concrete fragments"),
        *Definition->EquipmentId.ToString()))
    {
        return false;
    }

    WeaponMesh->SetSkeletalMeshAsset(Definition->EquipmentMesh);
    WeaponMesh->SetAnimInstanceClass(Definition->EquipmentAnimationClass);
    Instance.RuntimeData = FBBBEquipmentRuntimeData();
    if (!ensureMsgf(Definition->AmmoConfig.AmmoCapacity > 0, TEXT("[UBBBE]Ammo capacity must be positive")))
    {
        return false;
    }
    Instance.RuntimeData.Ammo.LoadedAmmo = Definition->AmmoConfig.AmmoCapacity;
    Instance.AnimationSystem.Reset();
    return true;
}
