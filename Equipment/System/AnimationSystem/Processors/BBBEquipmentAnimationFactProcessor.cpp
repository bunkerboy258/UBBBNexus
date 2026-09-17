#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/Processors/BBBEquipmentAnimationFactProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/Definition/BBBEquipmentAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/Fragment/BBBEquipmentEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Fragment/BBBEquipmentFireFragment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBEquipmentAnimationFactProcessor::Update(
    USkeletalMeshComponent &CharacterMesh, USkeletalMeshComponent &WeaponMesh,
    FBBBEquipmentAnimationRuntimeData &Data, const FBBBEquipmentFireRuntimeData &Fire,
    const FBBBEquipmentEquipFragment &EquipFragment, const FBBBEquipmentFireFragment &FireFragment) const
{
    UBBBEquipmentAnimInstance *AnimInstance = Cast<UBBBEquipmentAnimInstance>(WeaponMesh.GetAnimInstance());
    UWorld *World = WeaponMesh.GetWorld();
    if (!ensureMsgf(AnimInstance && World, TEXT("[UBBBE]Equipment animation snapshot dependencies are invalid")))
    {
        return;
    }

    FBBBEquipmentAnimationFacts &Facts = Data.Facts;
    Facts = FBBBEquipmentAnimationFacts();
    Facts.CurrentWorldTimeSeconds = World->GetTimeSeconds();
    Facts.LastFireTimeSeconds = Fire.LastFireTimeSeconds;
    Facts.FireSequence = Fire.FireSequence;
    Facts.LoadedAmmo = Fire.LoadedAmmo;
    Facts.AmmoCapacity = Fire.AmmoCapacity;

    const FName RightHandBone = TEXT("hand_r");
    const bool bHasRightHandBone = CharacterMesh.GetBoneIndex(RightHandBone) != INDEX_NONE;
    Facts.bHasValidAimSource = bHasRightHandBone
        && WeaponMesh.DoesSocketExist(FireFragment.GetMuzzleSocketName());
    if (Facts.bHasValidAimSource)
    {
        Facts.AimSourceLocalTransform = WeaponMesh.GetSocketTransform(
            FireFragment.GetMuzzleSocketName(), RTS_World).GetRelativeTransform(
                CharacterMesh.GetBoneTransform(RightHandBone, RTS_World));
    }

    Facts.bHasLeftHandTarget = bHasRightHandBone
        && WeaponMesh.DoesSocketExist(EquipFragment.GetLeftHandSocketName());
    if (Facts.bHasLeftHandTarget)
    {
        const FVector SocketPosition = WeaponMesh.GetSocketTransform(
            EquipFragment.GetLeftHandSocketName(), RTS_Component).GetLocation();
        const FVector TargetWorld = WeaponMesh.GetComponentTransform().TransformPosition(
            SocketPosition + EquipFragment.GetLeftHandSocketOffset());
        Facts.LeftHandTargetHandRSpace = CharacterMesh.GetBoneTransform(
            RightHandBone, RTS_World).InverseTransformPosition(TargetWorld) + EquipFragment.GetLeftHandIKOffset();
    }

    ensureMsgf(Facts.bHasValidAimSource, TEXT("[UBBBE]Equipment muzzle or character hand_r bone is missing"));
    ensureMsgf(Facts.bHasLeftHandTarget, TEXT("[UBBBE]Weapon left hand socket or character hand_r bone is missing"));
    AnimInstance->PublishAnimationFacts(Facts);
}
