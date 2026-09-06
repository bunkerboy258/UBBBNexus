#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Fragment/BBBEquipFragment.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBEquipmentPose, Log, All);

namespace
{
bool TryBuildSocketBoneSpaceTransform(
    USkeletalMeshComponent &CharacterMesh,
    UStaticMeshComponent &EquipmentMesh,
    FName ReferenceBoneName,
    FName EquipmentSocketName,
    const FVector &SocketOffset,
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

    FTransform SocketComponent = EquipmentMesh.GetSocketTransform(
        EquipmentSocketName,
        RTS_Component);
    SocketComponent.AddToTranslation(SocketOffset);
    const FTransform SocketWorld = EquipmentMesh.GetComponentTransform() * SocketComponent;
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

    const FName RightHandBoneName = CharacterMesh.GetSocketBoneName(AttachmentSocketName);
    RuntimeData.AimSourceRightHandBoneSpace = FTransform::Identity;
    RuntimeData.bHasValidAimSource = false;
    RuntimeData.LeftHandIKBaseTargetRightHandBoneSpace = FTransform::Identity;
    RuntimeData.CharacterMesh = &CharacterMesh;
    RuntimeData.RightHandBoneName = RightHandBoneName;
    RuntimeData.bHasValidLeftHandIKTarget = false;

    UStaticMeshComponent *EquipmentMesh = PresentationActor->GetEquipmentMesh();
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
        FVector CachedLeftHandGripSocketOffset = LeftHandGripSocketOffset;
        if (bRefreshLeftHandGripSocketOffsetEveryFrame)
        {
            CachedLeftHandGripSocketOffset = FVector::ZeroVector;
            UE_LOG(
                LogBBBEquipmentPose,
                Warning,
                TEXT("[UBBBE]Left hand grip socket offset refresh is enabled for debugging only"));
        }

        RuntimeData.bHasValidAimSource = TryBuildSocketBoneSpaceTransform(
            CharacterMesh,
            *EquipmentMesh,
            RightHandBoneName,
            AimSourceSocketName,
            FVector::ZeroVector,
            RuntimeData.AimSourceRightHandBoneSpace);

        if (!LeftHandGripSocketName.IsNone())
        {
            RuntimeData.bHasValidLeftHandIKTarget = TryBuildSocketBoneSpaceTransform(
                CharacterMesh,
                *EquipmentMesh,
                RightHandBoneName,
                LeftHandGripSocketName,
                CachedLeftHandGripSocketOffset,
                RuntimeData.LeftHandIKBaseTargetRightHandBoneSpace);
        }
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
