#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireResults.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SkeletalMeshComponent.h"

bool UBBBEquipmentSystem::Initialize(
    UBBBEquipmentInstance &InInstance,
    UBBBEquipmentDefinition &InDefinition,
    UBBBEquipmentRuntimeData &InRuntimeData)
{
    Instance = &InInstance;
    Definition = &InDefinition;
    RuntimeData = &InRuntimeData;

    if (!ensureMsgf(
        RuntimeData->GetEquip()
            && (!Definition->FireDomin.IsValid() || RuntimeData->GetFire()),
        TEXT("[UBBBE]Equipment fragment runtime data is incomplete")))
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::Equip(
    USkeletalMeshComponent &CharacterMesh,
    FName AttachmentSocketName)
{
    if (!ensureMsgf(Instance, TEXT("[UBBBE]Equipment system has no owning instance")))
    {
        return false;
    }

    if (Instance->PresentationActor)
    {
        return true;
    }

    if (!ensureMsgf(Definition && Definition->EquipDomin.IsValid() && RuntimeData && RuntimeData->GetEquip(), TEXT("[UBBBE]Equipment system cannot equip")))
    {
        return false;
    }

    Instance->PresentationActor = Definition->EquipDomin.Get().Equip(
        *RuntimeData->GetEquip(),
        CharacterMesh,
        AttachmentSocketName);

    UBBBEquipRuntimeData *EquipRuntimeData = RuntimeData->GetEquip();
    if (!ensureMsgf(EquipRuntimeData, TEXT("[UBBBE]Equipment equip runtime data is unavailable after equip")))
    {
        return false;
    }

    Instance->SetLeftHandIKOffsetRightHand(
        EquipRuntimeData->LeftHandIKOffsetRightHand,
        EquipRuntimeData->bHasValidLeftHandIKOffset);

    if (!Instance->PresentationActor)
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::Fire(FBBBEquipmentFireResult &OutResult)
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData && Definition->FireDomin.IsValid() && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire domin is unavailable")))
    {
        return false;
    }

    if (!Definition->FireDomin.Get().Fire(
        *Instance->PresentationActor,
        *RuntimeData->GetFire(),
        OutResult))
    {
        return false;
    }

    Instance->PresentationActor->NotifyFire(
        Instance->PresentationActor->GetWorld()->GetTimeSeconds());

    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::CanStartReload() const
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition, TEXT("[UBBBE]Equipment reload presentation is unavailable")))
    {
        return false;
    }

    return Definition->ReloadDuration > 0.0f;
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::BeginReload(const float WorldTimeSeconds)
{
    if (!ensureMsgf(Instance && Instance->PresentationActor, TEXT("[UBBBE]Equipment reload start presentation is unavailable")))
    {
        return;
    }

    Instance->PresentationActor->NotifyReloadStarted(
        WorldTimeSeconds,
        GetReloadDuration());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::PresentFire()
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData && Definition->FireDomin.IsValid() && RuntimeData->GetFire(), TEXT("[UBBBE]Equipment fire domin is unavailable during present fire")))
    {
        return;
    }

    Definition->FireDomin.Get().Present(
        *Instance->PresentationActor,
        *RuntimeData->GetFire());

    Instance->PresentationActor->NotifyFire(
        Instance->PresentationActor->GetWorld()->GetTimeSeconds());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::CompleteReload(const float WorldTimeSeconds)
{
    if (!ensureMsgf(Instance && Instance->PresentationActor, TEXT("[UBBBE]Equipment reload completion presentation is unavailable")))
    {
        return;
    }

    Instance->PresentationActor->NotifyReloadCompleted(WorldTimeSeconds);
}

//------------------------------------------------------------------------------

float UBBBEquipmentSystem::GetEquipDuration() const
{
    if (!ensureMsgf(Definition && Definition->EquipDomin.IsValid(), TEXT("[UBBBE]Equipment equip duration is unavailable")))
    {
        return 0.0f;
    }

    return Definition->EquipDomin.Get().GetEquipDuration();
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::BuildEquipActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const
{
    OutPresentation.Montage = nullptr;
    OutPresentation.PlayRate = 1.0f;

    if (!ensureMsgf(Definition && Definition->EquipDomin.IsValid(), TEXT("[UBBBE]Equipment equip action presentation is unavailable")))
    {
        return;
    }

    Definition->EquipDomin.Get().BuildEquipActionPresentation(OutPresentation);
}

//------------------------------------------------------------------------------

float UBBBEquipmentSystem::GetReloadDuration() const
{
    if (!ensureMsgf(Definition, TEXT("[UBBBE]Equipment reload duration is unavailable")))
    {
        return 0.0f;
    }

    return FMath::Max(Definition->ReloadDuration, 0.01f);
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::BuildReloadActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const
{
    OutPresentation.Montage = nullptr;
    OutPresentation.PlayRate = 1.0f;

    if (!ensureMsgf(Definition, TEXT("[UBBBE]Equipment reload action presentation is unavailable")))
    {
        return;
    }

    OutPresentation.Montage = Definition->ReloadMontage;
    OutPresentation.PlayRate = 1.0f;

    if (!Definition->ReloadMontage)
    {
        return;
    }

    OutPresentation.PlayRate = FMath::Max(
        Definition->ReloadMontage->GetPlayLength() / GetReloadDuration(),
        0.01f);
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::BuildFireActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const
{
    OutPresentation.Montage = nullptr;
    OutPresentation.PlayRate = 1.0f;

    if (!ensureMsgf(Definition && Definition->FireDomin.IsValid(), TEXT("[UBBBE]Equipment fire action presentation is unavailable")))
    {
        return;
    }

    Definition->FireDomin.Get().BuildFireActionPresentation(OutPresentation);
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::ReleasePresentation()
{
    if (!Instance || !Instance->PresentationActor)
    {
        return;
    }

    Instance->PresentationActor->Destroy();
    Instance->PresentationActor = nullptr;
}

bool UBBBEquipmentSystem::TryGetAimSourceRightHandBoneSpace(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;

    const UBBBEquipRuntimeData *EquipRuntimeData = nullptr;
    if (RuntimeData)
    {
        EquipRuntimeData = RuntimeData->GetEquip();
    }
    if (!EquipRuntimeData || !EquipRuntimeData->bHasValidAimSource)
    {
        return false;
    }

    OutTransform = EquipRuntimeData->AimSourceRightHandBoneSpace;
    return true;
}

float UBBBEquipmentSystem::GetTimeSinceLastFire(const float WorldTimeSeconds) const
{
    const UBBBFireRuntimeData *FireRuntimeData = RuntimeData
        ? RuntimeData->GetFire()
        : nullptr;
    if (!FireRuntimeData)
    {
        return BIG_NUMBER;
    }

    return FMath::Max(WorldTimeSeconds - FireRuntimeData->GetLastFireTime(), 0.0f);
}
