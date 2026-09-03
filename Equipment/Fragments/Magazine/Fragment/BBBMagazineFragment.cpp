#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Fragment/BBBMagazineFragment.h"

#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Definition/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/Magazine/BBBMagazinePresentationActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

UBBBMagazineRuntimeData *FBBBMagazineFragment::InitializeRuntimeData(UObject &Outer) const
{
    UBBBMagazineRuntimeData *RuntimeData = NewObject<UBBBMagazineRuntimeData>(&Outer);
    if (!ensureMsgf(RuntimeData, TEXT("[UBBBE]Magazine runtime data creation failed")))
    {
        return nullptr;
    }

    RuntimeData->MagazineAmmo = MagazineSize;
    RuntimeData->ReserveAmmo = FMath::Max(0, InitialReserveAmmo);
    return RuntimeData;
}

bool FBBBMagazineFragment::CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const
{
    return RuntimeData.MagazineAmmo > 0;
}

void FBBBMagazineFragment::ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const
{
    RuntimeData.MagazineAmmo = FMath::Max(0, RuntimeData.MagazineAmmo - 1);
}

bool FBBBMagazineFragment::CanStartReload(const UBBBMagazineRuntimeData &RuntimeData) const
{
    return RuntimeData.MagazineAmmo < MagazineSize
        && RuntimeData.ReserveAmmo > 0;
}

void FBBBMagazineFragment::CommitReload(UBBBMagazineRuntimeData &RuntimeData) const
{
    const int32 NeededAmmo = FMath::Max(0, MagazineSize - RuntimeData.MagazineAmmo);
    const int32 LoadedAmmo = FMath::Min(NeededAmmo, RuntimeData.ReserveAmmo);

    RuntimeData.MagazineAmmo += LoadedAmmo;
    RuntimeData.ReserveAmmo -= LoadedAmmo;
}

float FBBBMagazineFragment::GetReloadDuration() const
{
    return FMath::Max(ReloadDuration, 0.01f);
}

float FBBBMagazineFragment::GetMagazineRemoveNormalizedTime() const
{
    return FMath::Clamp(MagazineRemoveNormalizedTime, 0.0f, 1.0f);
}

float FBBBMagazineFragment::GetMagazineSpawnNormalizedTime() const
{
    return FMath::Clamp(MagazineSpawnNormalizedTime, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------

bool FBBBMagazineFragment::SpawnMagazine(
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    if (RuntimeData.LoadedMagazineActor)
    {
        return true;
    }

    UStaticMeshComponent *EquipmentMesh = PresentationActor.GetEquipmentMesh();
    UWorld *World = PresentationActor.GetWorld();

    if (!ensureMsgf(
        MagazinePresentationActorClass
            && EquipmentMesh
            && World
            && EquipmentMesh->DoesSocketExist(MagazineSocketName),
        TEXT("[UBBBE]Magazine presentation configuration is incomplete")))
    {
        return false;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = &PresentationActor;
    SpawnParameters.Instigator = PresentationActor.GetInstigator();
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABBBMagazinePresentationActor *MagazineActor = World->SpawnActor<ABBBMagazinePresentationActor>(
        MagazinePresentationActorClass,
        FTransform::Identity,
        SpawnParameters);

    if (!ensureMsgf(MagazineActor, TEXT("[UBBBE]Magazine presentation actor creation failed")))
    {
        return false;
    }

    MagazineActor->PrepareForAttachment();
    MagazineActor->AttachToComponent(
        EquipmentMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        MagazineSocketName);
    MagazineActor->SetActorRelativeTransform(MagazineSocketOffset);

    RuntimeData.LoadedMagazineActor = MagazineActor;
    return true;
}

//------------------------------------------------------------------------------

void FBBBMagazineFragment::RemoveMagazine(UBBBMagazineRuntimeData &RuntimeData) const
{
    ABBBMagazinePresentationActor *MagazineActor = RuntimeData.LoadedMagazineActor;
    if (!MagazineActor)
    {
        return;
    }

    RuntimeData.LoadedMagazineActor = nullptr;
    MagazineActor->Drop(DroppedMagazineLifeSeconds);
}

//------------------------------------------------------------------------------

void FBBBMagazineFragment::DestroyLoadedMagazine(UBBBMagazineRuntimeData &RuntimeData) const
{
    if (!RuntimeData.LoadedMagazineActor)
    {
        return;
    }

    RuntimeData.LoadedMagazineActor->Destroy();
    RuntimeData.LoadedMagazineActor = nullptr;
}
