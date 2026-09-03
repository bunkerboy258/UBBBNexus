#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Fragment/BBBEquipFragment.h"

#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

namespace
{
bool TryBuildSocketBoneSpaceTransform(
    USkeletalMeshComponent &CharacterMesh,
    UStaticMeshComponent &EquipmentMesh,
    FName ReferenceBoneName,
    FName EquipmentSocketName,
    const FTransform &SocketOffset,
    FTransform &OutTransform)
{
    OutTransform = FTransform::Identity;

    if (!ensureMsgf(
        EquipmentMesh.DoesSocketExist(EquipmentSocketName),
        TEXT("[UBBBE]Equipment pose socket '%s' is missing"),
        *EquipmentSocketName.ToString()))
    {
        return false;
    }

    const FTransform SocketWorld = SocketOffset
        * EquipmentMesh.GetSocketTransform(EquipmentSocketName, RTS_World);
    const FTransform ReferenceBoneWorld = CharacterMesh.GetBoneTransform(ReferenceBoneName, RTS_World);
    OutTransform = SocketWorld.GetRelativeTransform(ReferenceBoneWorld);
    return true;
}
}

UBBBEquipRuntimeData *FBBBEquipFragment::InitializeRuntimeData(UObject &Outer) const
{
    UBBBEquipRuntimeData *RuntimeData = NewObject<UBBBEquipRuntimeData>(&Outer);
    if (!ensureMsgf(RuntimeData, TEXT("[UBBBE]Equip runtime data creation failed")))
    {
        return nullptr;
    }

    return RuntimeData;
}

ABBBEquipmentPresentationActor *FBBBEquipFragment::Equip(
    UBBBEquipRuntimeData &RuntimeData,
    USkeletalMeshComponent &CharacterMesh,
    FName AttachmentSocketName) const
{
    if (!ensureMsgf(
        PresentationActorClass && CharacterMesh.GetOwner() && CharacterMesh.GetWorld(),
        TEXT("[UBBBE]Equipment presentation dependencies are invalid")))
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = CharacterMesh.GetOwner();
    SpawnParameters.Instigator = Cast<APawn>(CharacterMesh.GetOwner());
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABBBEquipmentPresentationActor *PresentationActor = CharacterMesh.GetWorld()->SpawnActor<ABBBEquipmentPresentationActor>(
        PresentationActorClass,
        FTransform::Identity,
        SpawnParameters);

    if (!ensureMsgf(PresentationActor, TEXT("[UBBBE]Equipment presentation actor creation failed")))
    {
        return nullptr;
    }

    PresentationActor->AttachToComponent(
        &CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        AttachmentSocketName);

    PresentationActor->SetActorRelativeTransform(SpawnOffset);

    RuntimeData.AimSourceRightHandBoneSpace = FTransform::Identity;
    RuntimeData.bHasValidAimSource = false;

    UStaticMeshComponent *EquipmentMesh = PresentationActor->GetEquipmentMesh();
    const FName RightHandBoneName = CharacterMesh.GetSocketBoneName(AttachmentSocketName);
    const bool bHasValidReferenceBone = EquipmentMesh
        && RightHandBoneName != NAME_None
        && CharacterMesh.GetBoneIndex(RightHandBoneName) != INDEX_NONE;
    if (!ensureMsgf(
        bHasValidReferenceBone,
        TEXT("[UBBBE]Equipment pose reference bone is invalid")))
    {
        RuntimeData.AimSourceRightHandBoneSpace = FTransform::Identity;
    }

    if (bHasValidReferenceBone)
    {
        RuntimeData.bHasValidAimSource = TryBuildSocketBoneSpaceTransform(
            CharacterMesh,
            *EquipmentMesh,
            RightHandBoneName,
            AimSourceSocketName,
            FTransform::Identity,
            RuntimeData.AimSourceRightHandBoneSpace);
    }

    return PresentationActor;
}

float FBBBEquipFragment::GetEquipDuration() const
{
    return FMath::Max(EquipDuration, 0.01f);
}
