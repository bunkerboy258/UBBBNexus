
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Fragments/BBBItemFragment.h"
#include "BBBMagazineFragment.generated.h"
class ABBBWeaponActor;
class UAnimMontage;
class UBBBItemFragmentRuntimeData;
class UBBBMagazineRuntimeData;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBMagazineFragment final : public UBBBItemFragment
{
    GENERATED_BODY()
public:

    virtual UBBBItemFragmentRuntimeData *InitializeRuntimeData(UObject &Outer) const override;

    bool CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const;

    void ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const;

    bool Reload(ABBBWeaponActor &Weapon, UBBBMagazineRuntimeData &RuntimeData) const;

    void PresentReload(ABBBWeaponActor &Weapon, UBBBMagazineRuntimeData &RuntimeData) const;

    void Update(ABBBWeaponActor &Weapon, UBBBMagazineRuntimeData &RuntimeData) const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Ammo", meta = (ClampMin = "1"))
    int32 MagazineSize = 30;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Ammo", meta = (ClampMin = "0"))
    int32 InitialReserveAmmo = 90;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation")
    TObjectPtr<UAnimMontage> ReloadMontage = nullptr;
};
