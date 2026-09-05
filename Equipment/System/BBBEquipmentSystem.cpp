#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireResults.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Fragment/BBBEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Definition/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"

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
            && (!Definition->FireDomin.IsValid() || RuntimeData->GetFire())
            && (!Definition->MagazineDomin.IsValid() || RuntimeData->GetMagazine()),
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

    if (!Instance->PresentationActor)
    {
        return false;
    }

    if (Definition->MagazineDomin.IsValid()
        && !Definition->MagazineDomin.Get().SpawnMagazine(
            *Instance->PresentationActor,
            *RuntimeData->GetMagazine()))
    {
        Instance->PresentationActor->Destroy();
        Instance->PresentationActor = nullptr;
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

    if (Definition->MagazineDomin.IsValid())
    {
        if (!ensureMsgf(RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine runtime data is unavailable during fire")))
        {
            return false;
        }

        if (!Definition->MagazineDomin.Get().CanConsumeRound(*RuntimeData->GetMagazine()))
        {
            return false;
        }
    }

    if (!Definition->FireDomin.Get().Fire(
        *Instance->PresentationActor,
        *RuntimeData->GetFire(),
        OutResult))
    {
        return false;
    }

    if (Definition->MagazineDomin.IsValid())
    {
        Definition->MagazineDomin.Get().ConsumeRound(*RuntimeData->GetMagazine());
    }

    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::CanStartReload() const
{
    if (!ensureMsgf(Instance && Instance->PresentationActor && Definition && RuntimeData && Definition->MagazineDomin.IsValid() && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine domin is unavailable")))
    {
        return false;
    }

    return Definition->MagazineDomin.Get().CanStartReload(*RuntimeData->GetMagazine());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::CommitReload()
{
    if (!ensureMsgf(Definition && RuntimeData && Definition->MagazineDomin.IsValid() && RuntimeData->GetMagazine(), TEXT("[UBBBE]Equipment magazine domin is unavailable during commit reload")))
    {
        return;
    }

    Definition->MagazineDomin.Get().CommitReload(*RuntimeData->GetMagazine());
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

float UBBBEquipmentSystem::GetReloadDuration() const
{
    if (!ensureMsgf(Definition && Definition->MagazineDomin.IsValid(), TEXT("[UBBBE]Equipment reload duration is unavailable")))
    {
        return 0.0f;
    }

    return Definition->MagazineDomin.Get().GetReloadDuration();
}

//------------------------------------------------------------------------------

float UBBBEquipmentSystem::GetMagazineRemoveNormalizedTime() const
{
    if (!ensureMsgf(Definition && Definition->MagazineDomin.IsValid(), TEXT("[UBBBE]Equipment magazine remove time is unavailable")))
    {
        return 0.0f;
    }

    return Definition->MagazineDomin.Get().GetMagazineRemoveNormalizedTime();
}

//------------------------------------------------------------------------------

float UBBBEquipmentSystem::GetMagazineSpawnNormalizedTime() const
{
    if (!ensureMsgf(Definition && Definition->MagazineDomin.IsValid(), TEXT("[UBBBE]Equipment magazine spawn time is unavailable")))
    {
        return 0.0f;
    }

    return Definition->MagazineDomin.Get().GetMagazineSpawnNormalizedTime();
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::ReleasePresentation()
{
    if (RuntimeData && RuntimeData->GetEquip())
    {
        UBBBEquipRuntimeData *EquipRuntimeData = RuntimeData->GetEquip();
        EquipRuntimeData->LeftHandIKBaseTargetRightHandBoneSpace = FTransform::Identity;
        EquipRuntimeData->LeftHandIKRuntimeSocketOffset = FTransform::Identity;
        EquipRuntimeData->bHasValidLeftHandIKTarget = false;
    }

    if (!Instance || !Instance->PresentationActor)
    {
        return;
    }

    if (Definition
        && RuntimeData
        && Definition->MagazineDomin.IsValid()
        && RuntimeData->GetMagazine())
    {
        Definition->MagazineDomin.Get().DestroyLoadedMagazine(*RuntimeData->GetMagazine());
    }

    Instance->PresentationActor->Destroy();
    Instance->PresentationActor = nullptr;
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::RemoveMagazine()
{
    if (!Definition || !RuntimeData || !Definition->MagazineDomin.IsValid() || !RuntimeData->GetMagazine())
    {
        return;
    }

    Definition->MagazineDomin.Get().RemoveMagazine(*RuntimeData->GetMagazine());
}

//------------------------------------------------------------------------------

void UBBBEquipmentSystem::SpawnMagazine()
{
    if (!Instance
        || !Instance->PresentationActor
        || !Definition
        || !RuntimeData
        || !Definition->MagazineDomin.IsValid()
        || !RuntimeData->GetMagazine())
    {
        return;
    }

    Definition->MagazineDomin.Get().SpawnMagazine(
        *Instance->PresentationActor,
        *RuntimeData->GetMagazine());
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

bool UBBBEquipmentSystem::TryGetLeftHandIKTargetRightHandBoneSpace(FTransform &OutTransform) const
{
    OutTransform = FTransform::Identity;

    const UBBBEquipRuntimeData *EquipRuntimeData = nullptr;
    if (RuntimeData)
    {
        EquipRuntimeData = RuntimeData->GetEquip();
    }
    if (!EquipRuntimeData || !EquipRuntimeData->bHasValidLeftHandIKTarget)
    {
        return false;
    }

    FTransform CurrentTarget = EquipRuntimeData->LeftHandIKBaseTargetRightHandBoneSpace;
    const FBBBEquipFragment *EquipFragment = Definition
        ? Definition->EquipDomin.GetPtr<FBBBEquipFragment>()
        : nullptr;
    if (EquipFragment)
    {
        if (EquipFragment->bRefreshLeftHandGripSocketOffsetEveryFrame)
        {
            CurrentTarget = CurrentTarget * EquipFragment->LeftHandGripSocketOffset;
        }
    }

    OutTransform = CurrentTarget * EquipRuntimeData->LeftHandIKRuntimeSocketOffset;
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
