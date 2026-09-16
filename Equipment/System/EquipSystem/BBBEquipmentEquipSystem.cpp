#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/BBBEquipmentEquipSystem.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Execution/BBBEquipmentCommandExecutor.h"
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
    if (!Instance.bIsMirror && Instance.RuntimeData.Reload.bIsReloading)
    {
        FBBBEquipmentCommand Command;
        Command.Type = EBBBEquipmentCommandType::CancelReload;
        Command.Sequence = Instance.RuntimeData.Reload.Sequence;
        FBBBEquipmentCommandExecutor::Execute(Instance, Command);
    }
    Instance.PendingCommands.Reset();
    Instance.RuntimeData.Equip = FBBBEquipmentEquipRuntimeData();
    Instance.RuntimeData.Reload.bIsReloading = false;
    Instance.bIsActive = false;
    Instance.SetActorHiddenInGame(true);
    Instance.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Instance.AnimationSystem.Reset();
}

bool FBBBEquipmentEquipSystem::SubmitEquipMontage(ABBBEquipmentInstance &Instance, const int32 Sequence) const
{
    if (!ensureMsgf(Instance.bIsActive && Instance.Definition && Instance.CharacterAPI,
        TEXT("[UBBBE]Equipment presentation dependencies are invalid")))
    {
        return false;
    }

    UAnimMontage *Montage = Instance.Definition->EquipConfig.EquipMontage;
    return !Montage || Instance.CharacterAPI->SubmitEquipmentMontage(Montage, 1.0f, Sequence);
}
