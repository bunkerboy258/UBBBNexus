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
    if (!ensureMsgf(CharacterMesh.IsValid() && WeaponMesh && Data && Fire && EquipFragment && FireFragment,
        TEXT("[UBBBE]Animation update dependencies are invalid")))
    {
        return;
    }

    Processor.Update(*CharacterMesh.Get(), *WeaponMesh, *Data, *Fire, *EquipFragment, *FireFragment);
}
