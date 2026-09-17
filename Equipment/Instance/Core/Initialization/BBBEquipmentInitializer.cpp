#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Initialization/BBBEquipmentInitializer.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBEquipmentInitializer::Initialize(ABBBEquipment &Instance,
    USkeletalMeshComponent &CharacterMesh, FBBBCharacterExternalAPI &CharacterAPI)
{
    // 先取得装备定义和装备网格
    UBBBEquipmentDefinition *Definition = Instance.Definition;
    USkeletalMeshComponent *WeaponMesh = Instance.EquipmentSkeletalMesh;
    if (!ensureMsgf(Definition && WeaponMesh, TEXT("[UBBBE]Equipment initialization dependencies are invalid")))
    {
        return false;
    }

    // 初始化要求装备资源和动画类型完整有效
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

    // 初始化要求装备片段使用具体实现
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

    // 将定义资源应用到装备网格
    WeaponMesh->SetSkeletalMeshAsset(Definition->EquipmentMesh);
    WeaponMesh->SetAnimInstanceClass(Definition->EquipmentAnimationClass);
    // 重置运行数据并建立初始弹药
    Instance.RuntimeData = FBBBEquipmentRuntimeData();
    if (!ensureMsgf(Definition->FireFragment.Get().AmmoCapacity > 0, TEXT("[UBBBE]Ammo capacity must be positive")))
    {
        return false;
    }
    FBBBEquipmentRuntimeData &Runtime = Instance.RuntimeData;
    Runtime.Fire.AmmoCapacity = Definition->FireFragment.Get().AmmoCapacity;
    Runtime.Fire.LoadedAmmo = Runtime.Fire.AmmoCapacity;
    const FName EquipmentId = Definition->EquipmentId;

    // 按固定顺序注入装备子系统依赖
    Instance.ExternalAPI.Initialize(Runtime.Input, Instance.bIsMirror);
    Instance.InputPipeline.Initialize(Runtime.Input, Runtime.Equip, Runtime.Fire, Runtime.Reload,
        CharacterAPI, EquipmentId, Instance.bIsMirror);
    Instance.EquipSystem.Initialize(Runtime.Equip, Runtime.Fire, Definition->EquipFragment.Get(),
        CharacterAPI, EquipmentId, Instance.bIsMirror);
    Instance.ReloadSystem.Initialize(Runtime.Reload, Runtime.Fire, Definition->ReloadFragment.Get(),
        CharacterAPI, EquipmentId, Instance.bIsMirror);
    Instance.FireSystem.Initialize(Instance, *WeaponMesh, Runtime.Fire, Runtime.Reload,
        Definition->FireFragment.Get(), CharacterAPI, EquipmentId, Instance.bIsMirror);
    Instance.AnimationSystem.Initialize(CharacterMesh, *WeaponMesh, Runtime.Animation, Runtime.Fire,
        Definition->EquipFragment.Get(), Definition->FireFragment.Get());
    // 最后注入装备更新管线
    Instance.UpdatePipeline.Initialize(Runtime, Instance.InputPipeline, Instance.EquipSystem,
        Instance.ReloadSystem, Instance.FireSystem, Instance.AnimationSystem);
    return true;
}
