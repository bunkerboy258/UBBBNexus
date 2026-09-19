#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBEquipmentEquipFragment.h"
#include "BBBStandardEquipFragment.generated.h"

class UAnimMontage;

/** 标准网格挂接与左手握持实现 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBStandardEquipFragment final : public FBBBEquipmentEquipFragment
{
    GENERATED_BODY()

    virtual FTransform GetSpawnOffset() const override
    {
        return SpawnOffset;
    }

    virtual bool SubmitMontage(ABBBCharacter &CharacterAPI, int32 Sequence) const override;

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
