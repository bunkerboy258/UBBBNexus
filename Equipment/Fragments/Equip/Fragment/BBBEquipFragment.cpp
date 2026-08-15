#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Fragment/BBBEquipFragment.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
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
    FBBBCharacterExternalAPI &CharacterAPI,
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
    RuntimeData.LeftHandTargetRightHandBoneSpace = FTransform::Identity;
    RuntimeData.bHasValidAimSource = false;
    RuntimeData.bHasValidLeftHandTarget = false;

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
        RuntimeData.LeftHandTargetRightHandBoneSpace = FTransform::Identity;
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

        RuntimeData.bHasValidLeftHandTarget = TryBuildSocketBoneSpaceTransform(
            CharacterMesh,
            *EquipmentMesh,
            RightHandBoneName,
            LeftHandGripSocketName,
            LeftHandGripSocketLocalOffset,
            RuntimeData.LeftHandTargetRightHandBoneSpace);
    }

    if (EquipMontage)
    {
        FBBBCharacterAnimationRequest Request;
        Request.Montage = EquipMontage;
        CharacterAPI.QueueMontage(Request);
    }

    RuntimeData.bIsEquipping = EquipMontage != nullptr;
    RuntimeData.EquipStartTime = 0.0f;
    RuntimeData.EquipEndTime = 0.0f;

    if (RuntimeData.bIsEquipping)
    {
        RuntimeData.EquipStartTime = CharacterMesh.GetWorld()->GetTimeSeconds();
        RuntimeData.EquipEndTime = RuntimeData.EquipStartTime + EquipMontage->GetPlayLength();
    }

    return PresentationActor;
}

void FBBBEquipFragment::Update(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBEquipRuntimeData &RuntimeData) const
{
    CharacterAPI.SubmitAimIKBlockRequest(RuntimeData.bIsEquipping);

    if (!RuntimeData.bIsEquipping)
    {
        return;
    }

    UWorld *World = PresentationActor.GetWorld();
    if (!ensureMsgf(World, TEXT("[UBBBE]Equipment presentation world is null")))
    {
        return;
    }

    const float CurrentTime = World->GetTimeSeconds();
    const float EquipDuration = RuntimeData.EquipEndTime - RuntimeData.EquipStartTime;
    float EquipProgress = 1.0f;

    if (EquipDuration > KINDA_SMALL_NUMBER)
    {
        EquipProgress = FMath::Clamp(
            (CurrentTime - RuntimeData.EquipStartTime) / EquipDuration,
            0.0f,
            1.0f);
    }

    if (EquipLeftHandIKAlphaCurve)
    {
        CharacterAPI.SubmitEquipmentLeftHandIKAlpha(
            EquipLeftHandIKAlphaCurve->GetFloatValue(EquipProgress));
    }

    if (CurrentTime < RuntimeData.EquipEndTime)
    {
        return;
    }

    RuntimeData.bIsEquipping = false;
    RuntimeData.EquipStartTime = 0.0f;
    RuntimeData.EquipEndTime = 0.0f;
}

bool FBBBEquipFragment::IsLeftHandIKEnabled() const
{
    return bEnableLeftHandIK;
}
