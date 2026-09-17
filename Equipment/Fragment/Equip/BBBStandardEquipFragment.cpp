#include "BBBWork/UBBBNexus/Equipment/Fragment/Equip/BBBStandardEquipFragment.h"

#include "BBBWork/UBBBNexus/Equipment/Fragment/Definition/BBBEquipmentFragmentContexts.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/Definition/Equip/BBBEquipmentEquipRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"

bool FBBBStandardEquipFragment::Activate(FBBBEquipmentEquipContext &Context) const
{
    if (!ensureMsgf(Context.CharacterMesh.DoesSocketExist(Context.AttachmentSocketName),
        TEXT("[UBBBE]Character equipment attachment socket '%s' is missing"),
        *Context.AttachmentSocketName.ToString()))
    {
        return false;
    }

    Context.Instance.AttachToComponent(
        &Context.CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        Context.AttachmentSocketName);
    Context.Instance.SetActorRelativeTransform(SpawnOffset);
    Context.Instance.SetActorHiddenInGame(false);

    const FName ReferenceBone = Context.CharacterMesh.GetSocketBoneName(Context.AttachmentSocketName);
    Context.Runtime.bHasValidAimSource = ReferenceBone != NAME_None
        && Context.CharacterMesh.GetBoneIndex(ReferenceBone) != INDEX_NONE
        && Context.WeaponMesh.DoesSocketExist(Context.MuzzleSocketName);
    Context.Runtime.AimSourceRightHandBoneSpace = FTransform::Identity;
    if (Context.Runtime.bHasValidAimSource)
    {
        const FTransform SocketWorld = Context.WeaponMesh.GetSocketTransform(Context.MuzzleSocketName, RTS_World);
        const FTransform BoneWorld = Context.CharacterMesh.GetBoneTransform(ReferenceBone, RTS_World);
        Context.Runtime.AimSourceRightHandBoneSpace = SocketWorld.GetRelativeTransform(BoneWorld);
    }

    if (!ensureMsgf(Context.Runtime.bHasValidAimSource,
        TEXT("[UBBBE]Equipment muzzle or right hand reference bone is missing")))
    {
        Context.Runtime.AimSourceRightHandBoneSpace = FTransform::Identity;
    }

    return true;
}

bool FBBBStandardEquipFragment::SubmitMontage(FBBBCharacterExternalAPI &CharacterAPI, const int32 Sequence) const
{
    return !EquipMontage || CharacterAPI.SubmitEquipmentMontage(EquipMontage, 1.0f, Sequence);
}
