
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBItemTypes.h"
#include "BBBWork/UBBBNexus/Item/Fragments/BBBItemFragment.h"
#include "BBBEquipmentPoseFragment.generated.h"
class UCurveFloat;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBEquipmentPoseFragment final : public UBBBItemFragment
{
    GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon")
    EBBBWeaponAnimType WeaponAnimType = EBBBWeaponAnimType::Rifle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Socket")
    FName AimSourceSocketName = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Socket")
    FName LeftHandGripSocketName = TEXT("LeftHand");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|IK")
    FTransform LeftHandGripSocketLocalOffset = FTransform::Identity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|IK")
    bool bEnableLeftHandIK = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|IK")
    TObjectPtr<UCurveFloat> EquipLeftHandIKAlphaCurve = nullptr;
};
