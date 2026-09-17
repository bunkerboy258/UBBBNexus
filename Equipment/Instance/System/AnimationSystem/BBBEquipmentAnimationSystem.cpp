#include "BBBWork/UBBBNexus/Equipment/Instance/System/AnimationSystem/BBBEquipmentAnimationSystem.h"

#include "Components/SkeletalMeshComponent.h"

void FBBBEquipmentAnimationSystem::Initialize(USkeletalMeshComponent &InCharacterMesh, USkeletalMeshComponent &InWeaponMesh,
        FBBBEquipmentAnimationRuntimeData &InData, const FBBBEquipmentFireRuntimeData &InFire,
        const FBBBEquipmentEquipFragment &InEquipFragment, const FBBBEquipmentFireFragment &InFireFragment)
{
    CharacterMesh = &InCharacterMesh;
    WeaponMesh = &InWeaponMesh;
    Data = &InData;
    Fire = &InFire;
    EquipFragment = &InEquipFragment;
    FireFragment = &InFireFragment;
}

void FBBBEquipmentAnimationSystem::Update() const
{
    // 装备动画更新需要角色网格装备网格和运行数据有效
    if (!ensureMsgf(CharacterMesh.IsValid() && WeaponMesh && Data && Fire && EquipFragment && FireFragment,
        TEXT("[UBBBE]Animation update dependencies are invalid")))
    {
        return;
    }

    // 根据装备状态生成当前动画事实
    Processor.Update(*CharacterMesh.Get(), *WeaponMesh, *Data, *Fire, *EquipFragment, *FireFragment);
}
