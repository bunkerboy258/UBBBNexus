#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Fragment/BBBEquipFragment.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

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

FName FBBBEquipFragment::GetAimSourceSocketName() const
{
    return AimSourceSocketName;
}

FName FBBBEquipFragment::GetLeftHandGripSocketName() const
{
    return LeftHandGripSocketName;
}

const FTransform &FBBBEquipFragment::GetLeftHandGripSocketLocalOffset() const
{
    return LeftHandGripSocketLocalOffset;
}

bool FBBBEquipFragment::IsLeftHandIKEnabled() const
{
    return bEnableLeftHandIK;
}

UAnimSequence *FBBBEquipFragment::GetEquippedUpperBodyAnimation() const
{
    return EquippedUpperBodyAnimation;
}
