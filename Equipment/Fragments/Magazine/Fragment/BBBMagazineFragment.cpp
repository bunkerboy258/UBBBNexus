#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Fragment/BBBMagazineFragment.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
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
    return !RuntimeData.bIsReloading && RuntimeData.MagazineAmmo > 0;
}

void FBBBMagazineFragment::ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const
{
    RuntimeData.MagazineAmmo = FMath::Max(0, RuntimeData.MagazineAmmo - 1);
}

bool FBBBMagazineFragment::Reload(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    if (RuntimeData.bIsReloading
        || RuntimeData.MagazineAmmo >= MagazineSize
        || RuntimeData.ReserveAmmo <= 0
        || !ReloadMontage
        || !PresentationActor.GetWorld())
    {
        return false;
    }

    RuntimeData.bIsReloading = true;
    RuntimeData.ReloadEndTime = PresentationActor.GetWorld()->GetTimeSeconds() + ReloadMontage->GetPlayLength();
    RuntimeData.ReloadIKBlockEndTime = RuntimeData.ReloadEndTime;

    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    CharacterAPI.QueueMontage(Request);
    return true;
}

void FBBBMagazineFragment::PresentReload(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    UWorld *World = PresentationActor.GetWorld();
    if (!ReloadMontage || !World)
    {
        return;
    }

    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    CharacterAPI.QueueMontage(Request);
    RuntimeData.ReloadIKBlockEndTime = World->GetTimeSeconds() + ReloadMontage->GetPlayLength();
    CharacterAPI.SubmitLeftHandIKBlockRequest(true);
    CharacterAPI.SubmitAimBlockRequest(true);
}

void FBBBMagazineFragment::Update(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    UWorld *World = PresentationActor.GetWorld();
    float CurrentTime = 0.0f;

    if (World)
    {
        CurrentTime = World->GetTimeSeconds();
    }

    const bool bBlockIK = World && CurrentTime < RuntimeData.ReloadIKBlockEndTime;
    CharacterAPI.SubmitLeftHandIKBlockRequest(bBlockIK);
    CharacterAPI.SubmitAimBlockRequest(bBlockIK);

    if (RuntimeData.ReloadIKBlockEndTime > 0.0f
        && CurrentTime >= RuntimeData.ReloadIKBlockEndTime)
    {
        RuntimeData.ReloadIKBlockEndTime = 0.0f;
    }

    if (!RuntimeData.bIsReloading
        || !World
        || CurrentTime < RuntimeData.ReloadEndTime)
    {
        return;
    }

    const int32 NeededAmmo = FMath::Max(0, MagazineSize - RuntimeData.MagazineAmmo);
    const int32 LoadedAmmo = FMath::Min(NeededAmmo, RuntimeData.ReserveAmmo);

    RuntimeData.MagazineAmmo += LoadedAmmo;
    RuntimeData.ReserveAmmo -= LoadedAmmo;
    RuntimeData.bIsReloading = false;
    RuntimeData.ReloadEndTime = 0.0f;
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
