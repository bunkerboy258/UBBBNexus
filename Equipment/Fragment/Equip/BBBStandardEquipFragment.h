#pragma once

#include "BBBWork/UBBBNexus/Equipment/Fragment/Equip/BBBEquipmentEquipFragment.h"
#include "BBBStandardEquipFragment.generated.h"

class UAnimMontage;

/** 标准网格挂接与左手握持实现 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBStandardEquipFragment final : public FBBBEquipmentEquipFragment
{
    GENERATED_BODY()

    virtual bool Activate(FBBBEquipmentEquipContext &Context) const override;
    virtual bool SubmitMontage(FBBBCharacterExternalAPI &CharacterAPI, int32 Sequence) const override;

    virtual FName GetLeftHandSocketName() const override
    {
        return LeftHandSocketName;
    }

    virtual FVector GetLeftHandSocketOffset() const override
    {
        return LeftHandSocketOffset;
    }

    virtual FVector GetLeftHandIKOffset() const override
    {
        return LeftHandIKOffset;
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    FTransform SpawnOffset = FTransform::Identity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    FName LeftHandSocketName = TEXT("LeftHand");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    FVector LeftHandSocketOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    FVector LeftHandIKOffset = FVector::ZeroVector;
};
