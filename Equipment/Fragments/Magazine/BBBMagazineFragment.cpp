#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineFragment.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/Definition/BBBMagazineRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Engine/World.h"

UBBBMagazineRuntimeData *UBBBMagazineFragment::InitializeRuntimeData(UObject &Outer) const
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

bool UBBBMagazineFragment::CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const
{
    return !RuntimeData.bIsReloading && RuntimeData.MagazineAmmo > 0;
}

void UBBBMagazineFragment::ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const
{
    RuntimeData.MagazineAmmo = FMath::Max(0, RuntimeData.MagazineAmmo - 1);
}

bool UBBBMagazineFragment::Reload(
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

    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    CharacterAPI.QueueMontage(Request);
    return true;
}

void UBBBMagazineFragment::PresentReload(FBBBCharacterExternalAPI &CharacterAPI) const
{
    if (!ReloadMontage)
    {
        return;
    }

    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    CharacterAPI.QueueMontage(Request);
}

void UBBBMagazineFragment::Update(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    CharacterAPI.SubmitEquipmentIKBlockRequest(RuntimeData.bIsReloading);

    if (!RuntimeData.bIsReloading
        || !PresentationActor.GetWorld()
        || PresentationActor.GetWorld()->GetTimeSeconds() < RuntimeData.ReloadEndTime)
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
