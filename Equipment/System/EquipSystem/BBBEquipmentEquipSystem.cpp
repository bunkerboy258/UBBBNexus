#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/BBBEquipmentEquipSystem.h"

#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentActionResult.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/BBBEquipmentAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

bool FBBBEquipmentEquipSystem::Activate(ABBBEquipmentInstance &Instance) const
{
    if (Instance.bIsActive)
    {
        return true;
    }

    USkeletalMeshComponent *CharacterMesh = Instance.HolderMesh.Get();
    USkeletalMeshComponent *WeaponMesh = Instance.EquipmentSkeletalMesh;
    UBBBEquipmentDefinition *Definition = Instance.Definition;
    if (!ensureMsgf(
        CharacterMesh && WeaponMesh && Definition && Definition->EquipmentMesh && Definition->EquipmentAnimationClass,
        TEXT("[UBBBE]Equipment activation dependencies are invalid")))
    {
        return false;
    }

    if (!ensureMsgf(
        CharacterMesh->DoesSocketExist(Instance.AttachmentSocketName),
        TEXT("[UBBBE]Character equipment attachment socket '%s' is missing"),
        *Instance.AttachmentSocketName.ToString()))
    {
        return false;
    }

    Instance.AttachToComponent(
        CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        Instance.AttachmentSocketName);
    Instance.SetActorRelativeTransform(Definition->EquipConfig.SpawnOffset);
    Instance.SetActorHiddenInGame(false);

    UBBBEquipmentAnimInstance *WeaponAnim = Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance());
    UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    if (!ensureMsgf(WeaponAnim && CharacterAnim, TEXT("[UBBBE]Equipped animation instances have invalid classes")))
    {
        Instance.SetActorHiddenInGame(true);
        Instance.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        return false;
    }

    Instance.BoundCharacterAnimInstance = CharacterAnim;
    Instance.BoundWeaponAnimInstance = WeaponAnim;
    CharacterAnim->BindWeaponAnimInstance(WeaponAnim);
    WeaponMesh->PrimaryComponentTick.AddPrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);

    const FName ReferenceBone = CharacterMesh->GetSocketBoneName(Instance.AttachmentSocketName);
    const FName MuzzleSocket = Definition->FireConfig.MuzzleSocketName;
    Instance.RuntimeData.Equip.bHasValidAimSource = ReferenceBone != NAME_None
        && CharacterMesh->GetBoneIndex(ReferenceBone) != INDEX_NONE
        && WeaponMesh->DoesSocketExist(MuzzleSocket);
    Instance.RuntimeData.Equip.AimSourceRightHandBoneSpace = FTransform::Identity;
    if (Instance.RuntimeData.Equip.bHasValidAimSource)
    {
        const FTransform SocketWorld = WeaponMesh->GetSocketTransform(MuzzleSocket, RTS_World);
        const FTransform BoneWorld = CharacterMesh->GetBoneTransform(ReferenceBone, RTS_World);
        Instance.RuntimeData.Equip.AimSourceRightHandBoneSpace = SocketWorld.GetRelativeTransform(BoneWorld);
    }

    if (!ensureMsgf(Instance.RuntimeData.Equip.bHasValidAimSource, TEXT("[UBBBE]Equipment muzzle or right hand reference bone is missing")))
    {
        Instance.RuntimeData.Equip.AimSourceRightHandBoneSpace = FTransform::Identity;
    }

    Instance.bIsActive = true;
    return true;
}

void FBBBEquipmentEquipSystem::Deactivate(ABBBEquipmentInstance &Instance) const
{
    UBBBAnimInstance *CharacterAnim = Instance.BoundCharacterAnimInstance.Get();
    UBBBEquipmentAnimInstance *WeaponAnim = Instance.BoundWeaponAnimInstance.Get();
    USkeletalMeshComponent *CharacterMesh = Instance.HolderMesh.Get();
    USkeletalMeshComponent *WeaponMesh = Instance.EquipmentSkeletalMesh;
    if (CharacterMesh && WeaponMesh)
    {
        WeaponMesh->PrimaryComponentTick.RemovePrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
    }

    if (CharacterAnim && CharacterAnim->TryGetWeaponAnimInstance() == WeaponAnim)
    {
        CharacterAnim->BindWeaponAnimInstance(nullptr);
    }

    Instance.BoundCharacterAnimInstance.Reset();
    Instance.BoundWeaponAnimInstance.Reset();
    Instance.RuntimeData.Equip = FBBBEquipmentEquipRuntimeData();
    Instance.RuntimeData.Reload.bIsReloading = false;
    Instance.bIsActive = false;
    Instance.SetActorHiddenInGame(true);
    Instance.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Instance.AnimationSystem.Reset();
}

bool FBBBEquipmentEquipSystem::BeginAction(
    ABBBEquipmentInstance &Instance,
    const int32 Sequence,
    const float DurationOverride,
    FBBBEquipmentActionResult &OutResult) const
{
    const UBBBEquipmentDefinition *Definition = Instance.Definition;
    if (!ensureMsgf(Instance.bIsActive && Definition, TEXT("[UBBBE]Equipment action requires active equipment")))
    {
        return false;
    }

    OutResult = FBBBEquipmentActionResult();
    OutResult.DurationSeconds = DurationOverride > 0.0f
        ? DurationOverride
        : FMath::Max(Definition->EquipConfig.EquipDuration, 0.01f);
    OutResult.Presentation.Montage = Definition->EquipConfig.EquipMontage;
    if (OutResult.Presentation.Montage)
    {
        OutResult.Presentation.PlayRate = FMath::Max(
            OutResult.Presentation.Montage->GetPlayLength() / OutResult.DurationSeconds,
            0.01f);
    }

    Instance.RecordAction(EBBBEquipmentActionType::Equip, Sequence, OutResult);
    return true;
}
