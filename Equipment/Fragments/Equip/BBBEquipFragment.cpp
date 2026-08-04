#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipFragment.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UBBBEquipRuntimeData *UBBBEquipFragment::InitializeRuntimeData(UObject &Outer) const
{
    UBBBEquipRuntimeData *RuntimeData = NewObject<UBBBEquipRuntimeData>(&Outer);
    if (!ensureMsgf(RuntimeData, TEXT("[UBBBE]Equip runtime data creation failed")))
    {
        return nullptr;
    }

    return RuntimeData;
}

ABBBEquipmentPresentationActor *UBBBEquipFragment::Equip(
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
    RuntimeData.EquipEndTime = RuntimeData.bIsEquipping
        ? CharacterMesh.GetWorld()->GetTimeSeconds() + EquipMontage->GetPlayLength()
        : 0.0f;

    return PresentationActor;
}

void UBBBEquipFragment::Update(
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBEquipRuntimeData &RuntimeData) const
{
    if (!RuntimeData.bIsEquipping)
    {
        return;
    }

    if (!ensureMsgf(PresentationActor.GetWorld(), TEXT("[UBBBE]Equipment presentation world is null")))
    {
        return;
    }

    if (PresentationActor.GetWorld()->GetTimeSeconds() < RuntimeData.EquipEndTime)
    {
        return;
    }

    RuntimeData.bIsEquipping = false;
    RuntimeData.EquipEndTime = 0.0f;
}
