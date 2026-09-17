#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/BBBEquipmentEquipSystem.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Definition/BBBEquipmentFragmentContexts.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Fire/BBBEquipmentFireFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Equip/BBBEquipmentEquipFragment.h"
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
    const UBBBEquipmentDefinition *Definition = Instance.Definition;
    if (!ensureMsgf(CharacterMesh && WeaponMesh && Definition && Instance.CharacterAPI
        && Definition->EquipFragment.IsValid() && Definition->FireFragment.IsValid(),
        TEXT("[UBBBE]Equipment activation dependencies are invalid")))
    {
        return false;
    }

    FBBBEquipmentEquipContext Context{
        Instance, *CharacterMesh, *WeaponMesh, Instance.RuntimeData.Equip,
        Instance.AttachmentSocketName, Definition->FireFragment.Get().GetMuzzleSocketName()};
    if (!Definition->EquipFragment.Get().Activate(Context))
    {
        return false;
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

    if (!ensureMsgf(Instance.Definition->EquipFragment.IsValid(), TEXT("[UBBBE]Equip fragment is missing")))
    {
        return false;
    }

    return Instance.Definition->EquipFragment.Get().SubmitMontage(*Instance.CharacterAPI, Sequence);
}
