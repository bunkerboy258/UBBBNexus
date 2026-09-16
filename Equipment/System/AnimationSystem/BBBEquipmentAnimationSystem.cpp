#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/BBBEquipmentAnimationSystem.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentActionResult.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBEquipmentAnimationSystem::RecordAction(
    const EBBBEquipmentActionType Type,
    const int32 Sequence,
    const FBBBEquipmentActionResult &Result)
{
    Facts.EquipmentActionType = Type;
    Facts.EquipmentActionSequence = Sequence;
    Facts.EquipmentActionDuration = Result.DurationSeconds;
    Facts.EquipmentActionMontage = Result.Presentation.Montage;
    Facts.EquipmentActionPlayRate = Result.Presentation.PlayRate;

    if (Type == EBBBEquipmentActionType::Fire)
    {
        Facts.FireSequence++;
    }
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

    const FBBBEquipmentEquipConfig &Config = Definition->EquipConfig;
    Facts.bHasLeftHandTarget = CharacterMesh->GetBoneIndex(TEXT("hand_r")) != INDEX_NONE
        && WeaponMesh->DoesSocketExist(Config.LeftHandSocketName);
    Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    if (Facts.bHasLeftHandTarget)
    {
        const FVector SocketPosition = WeaponMesh->GetSocketTransform(
            Config.LeftHandSocketName,
            RTS_Component).GetLocation();
        const FVector TargetWorld = WeaponMesh->GetComponentTransform().TransformPosition(
            SocketPosition + Config.LeftHandSocketOffset);
        Facts.LeftHandTargetHandRSpace = CharacterMesh->GetBoneTransform(
            TEXT("hand_r"),
            RTS_World).InverseTransformPosition(TargetWorld) + Config.LeftHandIKOffset;
    }

    if (!ensureMsgf(Facts.bHasLeftHandTarget, TEXT("[UBBBE]Weapon left hand socket or character hand_r bone is missing")))
    {
        Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    }

    Facts.LastFireTimeSeconds = Instance.RuntimeData.Fire.LastFireTimeSeconds;
    Facts.bIsReloading = Instance.RuntimeData.Reload.bIsReloading;
    Facts.ReloadStartTimeSeconds = Instance.RuntimeData.Reload.StartTimeSeconds;
    Facts.ReloadDurationSeconds = Instance.RuntimeData.Reload.DurationSeconds;
    Facts.ReloadSequence = Instance.RuntimeData.Reload.Sequence;
    AnimInstance->PublishAnimationFacts(Facts);
}

void FBBBEquipmentAnimationSystem::Reset()
{
    Facts = FBBBEquipmentAnimationFacts();
}
