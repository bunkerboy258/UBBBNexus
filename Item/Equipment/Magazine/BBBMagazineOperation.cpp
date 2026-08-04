#include "BBBWork/UBBBNexus/Item/Equipment/Magazine/BBBMagazineOperation.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Magazine/BBBMagazineRuntimeData.h"
#include "Engine/World.h"

UBBBMagazineRuntimeData *UBBBMagazineOperation::InitializeRuntimeData(UObject &Outer) const
{
    UBBBMagazineRuntimeData *RuntimeData = NewObject<UBBBMagazineRuntimeData>(&Outer);
    if (!ensureMsgf(RuntimeData, TEXT("[UBBBI]Magazine runtime data creation failed")))
    {
        return nullptr;
    }

    RuntimeData->MagazineAmmo = MagazineSize;
    RuntimeData->ReserveAmmo = FMath::Max(0, InitialReserveAmmo);
    return RuntimeData;
}

bool UBBBMagazineOperation::CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const
{
    return !RuntimeData.bIsReloading && RuntimeData.MagazineAmmo > 0;
}

void UBBBMagazineOperation::ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const
{
    RuntimeData.MagazineAmmo = FMath::Max(0, RuntimeData.MagazineAmmo - 1);
}

bool UBBBMagazineOperation::Reload(
    ABBBEquipmentActor &EquipmentActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    if (RuntimeData.bIsReloading
        || RuntimeData.MagazineAmmo >= MagazineSize
        || RuntimeData.ReserveAmmo <= 0
        || !ReloadMontage
        || !EquipmentActor.GetWorld())
    {
        return false;
    }

    RuntimeData.bIsReloading = true;
    RuntimeData.ReloadEndTime = EquipmentActor.GetWorld()->GetTimeSeconds() + ReloadMontage->GetPlayLength();

    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    EquipmentActor.QueueOperationMontage(Request);
    return true;
}

void UBBBMagazineOperation::PresentReload(ABBBEquipmentActor &EquipmentActor) const
{
    FBBBCharacterAnimationRequest Request;
    Request.Montage = ReloadMontage;
    EquipmentActor.QueueOperationMontage(Request);
}

void UBBBMagazineOperation::Update(
    ABBBEquipmentActor &EquipmentActor,
    UBBBMagazineRuntimeData &RuntimeData) const
{
    EquipmentActor.SubmitItemIKBlock(RuntimeData.bIsReloading);

    if (!RuntimeData.bIsReloading
        || !EquipmentActor.GetWorld()
        || EquipmentActor.GetWorld()->GetTimeSeconds() < RuntimeData.ReloadEndTime)
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
