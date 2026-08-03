
#include "BBBWork/UBBBNexus/Item/Fragments/Magazine/BBBMagazineFragment.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationCommands.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/BBBWeaponActor.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Magazine/BBBMagazineRuntimeData.h"

UBBBItemFragmentRuntimeData *UBBBMagazineFragment::InitializeRuntimeData(UObject &Outer) const
{

    UBBBMagazineRuntimeData *RuntimeData = NewObject<UBBBMagazineRuntimeData>(&Outer);

    RuntimeData->MagazineAmmo = FMath::Max(1, MagazineSize);

    RuntimeData->ReserveAmmo = FMath::Max(0, InitialReserveAmmo);
    return RuntimeData;
}

bool UBBBMagazineFragment::CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const
{

    return !RuntimeData.bIsReloading && RuntimeData.MagazineAmmo > 0;
}

void UBBBMagazineFragment::ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const
{

    --RuntimeData.MagazineAmmo;
}

bool UBBBMagazineFragment::Reload(
    ABBBWeaponActor &Weapon,
    UBBBMagazineRuntimeData &RuntimeData) const
{

    if (RuntimeData.bIsReloading
        || RuntimeData.MagazineAmmo >= MagazineSize
        || RuntimeData.ReserveAmmo <= 0
        || !ReloadMontage
        || !Weapon.GetWorld())
    {
        return false;
    }

    RuntimeData.bIsReloading = true;
    RuntimeData.bCommitReloadAmmo = true;
    RuntimeData.ReloadEndTime = Weapon.GetWorld()->GetTimeSeconds() + ReloadMontage->GetPlayLength();

    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    Request.bRestartIfPlaying = true;

    Weapon.SubmitMontage(Request);
    return true;
}

void UBBBMagazineFragment::PresentReload(ABBBWeaponActor &Weapon) const
{
    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    Request.bRestartIfPlaying = true;

    Weapon.SubmitMontage(Request);
}

void UBBBMagazineFragment::Update(
    ABBBWeaponActor &Weapon,
    UBBBMagazineRuntimeData &RuntimeData) const
{

    Weapon.SubmitItemIKBlock(RuntimeData.bIsReloading);

    if (!RuntimeData.bIsReloading
        || !Weapon.GetWorld()
        || Weapon.GetWorld()->GetTimeSeconds() < RuntimeData.ReloadEndTime)
    {
        return;
    }

    if (RuntimeData.bCommitReloadAmmo)
    {

        const int32 NeededAmmo = FMath::Max(0, MagazineSize - RuntimeData.MagazineAmmo);

        const int32 LoadedAmmo = FMath::Min(NeededAmmo, RuntimeData.ReserveAmmo);

        RuntimeData.MagazineAmmo += LoadedAmmo;

        RuntimeData.ReserveAmmo -= LoadedAmmo;
    }

    RuntimeData.bIsReloading = false;
    RuntimeData.bCommitReloadAmmo = false;
}
