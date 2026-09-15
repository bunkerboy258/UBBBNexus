#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Fragment/BBBEquipFragment.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

namespace
{
bool TryBuildSocketBoneSpaceTransform(
    USkeletalMeshComponent &CharacterMesh,
    USceneComponent &EquipmentComponent,
    FName ReferenceBoneName,
    FName EquipmentSocketName,
    const FVector &SocketOffset,
    FTransform &OutTransform)
{
    OutTransform = FTransform::Identity;

    if (!ensureMsgf(
        EquipmentComponent.DoesSocketExist(EquipmentSocketName),
        TEXT("[UBBBE]Equipment pose socket '%s' is missing"),
        *EquipmentSocketName.ToString()))
    {
        return false;
    }

    FTransform SocketComponent = EquipmentComponent.GetSocketTransform(
        EquipmentSocketName,
        RTS_Component);
    SocketComponent.AddToTranslation(SocketOffset);
    const FTransform SocketWorld = SocketComponent * EquipmentComponent.GetComponentTransform();
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
    PresentationActor->SetMuzzleSocketName(RuntimeData.MuzzleSocketName);

    const FName RightHandBoneName = CharacterMesh.GetSocketBoneName(AttachmentSocketName);
    RuntimeData.AimSourceRightHandBoneSpace = FTransform::Identity;
    RuntimeData.bHasValidAimSource = false;

    USceneComponent *EquipmentComponent = PresentationActor->GetEquipmentAttachmentComponent();
    const bool bHasValidReferenceBone = EquipmentComponent
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
            *EquipmentComponent,
            RightHandBoneName,
            RuntimeData.MuzzleSocketName,
            FVector::ZeroVector,
            RuntimeData.AimSourceRightHandBoneSpace);
    }

    return PresentationActor;
}

float FBBBEquipFragment::GetEquipDuration() const
{
    return FMath::Max(EquipDuration, 0.01f);
}

//------------------------------------------------------------------------------

void FBBBEquipFragment::BuildEquipActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const
{
    OutPresentation.Montage = EquipMontage;
    OutPresentation.PlayRate = 1.0f;

    if (!EquipMontage)
    {
        return;
    }

    OutPresentation.PlayRate = FMath::Max(
        EquipMontage->GetPlayLength() / GetEquipDuration(),
        0.01f);
}
