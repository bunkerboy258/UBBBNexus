#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/BBBEquipmentAnimationSystem.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBEquipmentAnimationSystem::RecordFire()
{
    Facts.FireSequence++;
}

void FBBBEquipmentAnimationSystem::Update(
    ABBBEquipmentInstance &Instance,
    const float WorldTimeSeconds)
{
    USkeletalMeshComponent *CharacterMesh = Instance.HolderMesh.Get();
    USkeletalMeshComponent *WeaponMesh = Instance.EquipmentSkeletalMesh;
    UBBBEquipmentAnimInstance *AnimInstance = WeaponMesh
        ? Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance())
        : nullptr;
    const UBBBEquipmentDefinition *Definition = Instance.Definition;
    if (!ensureMsgf(
        CharacterMesh && WeaponMesh && AnimInstance && Definition,
        TEXT("[UBBBE]Equipment animation snapshot dependencies are invalid")))
    {
        return;
    }

    Facts.CurrentWorldTimeSeconds = WorldTimeSeconds;
    Facts.bHasMainHandEquipment = Instance.bIsActive;
    Facts.bHasValidAimSource = Instance.RuntimeData.Equip.bHasValidAimSource;
    Facts.AimSourceLocalTransform = Facts.bHasValidAimSource
        ? Instance.RuntimeData.Equip.AimSourceRightHandBoneSpace
        : FTransform::Identity;

    if (!ensureMsgf(Definition->EquipFragment.IsValid(), TEXT("[UBBBE]Equip fragment is missing")))
    {
        return;
    }

    const FBBBEquipmentEquipFragment &Config = Definition->EquipFragment.Get();
    Facts.bHasLeftHandTarget = CharacterMesh->GetBoneIndex(TEXT("hand_r")) != INDEX_NONE
        && WeaponMesh->DoesSocketExist(Config.GetLeftHandSocketName());
    Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    if (Facts.bHasLeftHandTarget)
    {
        const FVector SocketPosition = WeaponMesh->GetSocketTransform(
            Config.GetLeftHandSocketName(),
            RTS_Component).GetLocation();
        const FVector TargetWorld = WeaponMesh->GetComponentTransform().TransformPosition(
            SocketPosition + Config.GetLeftHandSocketOffset());
        Facts.LeftHandTargetHandRSpace = CharacterMesh->GetBoneTransform(
            TEXT("hand_r"),
            RTS_World).InverseTransformPosition(TargetWorld) + Config.GetLeftHandIKOffset();
    }

    if (!ensureMsgf(Facts.bHasLeftHandTarget, TEXT("[UBBBE]Weapon left hand socket or character hand_r bone is missing")))
    {
        Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    }

    Facts.LastFireTimeSeconds = Instance.RuntimeData.Fire.LastFireTimeSeconds;
    Facts.LoadedAmmo = Instance.RuntimeData.Ammo.LoadedAmmo;
    Facts.AmmoCapacity = Definition->AmmoConfig.AmmoCapacity;
    AnimInstance->PublishAnimationFacts(Facts);
}

void FBBBEquipmentAnimationSystem::Reset()
{
    Facts = FBBBEquipmentAnimationFacts();
}
