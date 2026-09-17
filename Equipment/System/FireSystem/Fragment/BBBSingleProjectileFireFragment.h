#pragma once

#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Fragment/BBBEquipmentFireFragment.h"
#include "BBBSingleProjectileFireFragment.generated.h"

class ABBBBulletActor;
class UAnimMontage;
class USoundBase;

/** 单投射物开火实现 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBSingleProjectileFireFragment final : public FBBBEquipmentFireFragment
{
    GENERATED_BODY()

    FBBBSingleProjectileFireFragment();

    virtual bool CanFire(int32 LoadedAmmo, float LastFireTimeSeconds, float WorldTime) const override;
    virtual bool Fire(FBBBEquipmentFireContext &Context) const override;

    virtual FName GetMuzzleSocketName() const override
    {
        return MuzzleSocketName;
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.01"))
    float FireInterval = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    TSubclassOf<ABBBBulletActor> BulletActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.01"))
    float MuzzleSpeed = 30000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    FName MuzzleSocketName = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    TObjectPtr<USoundBase> FireSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire|Animation")
    TObjectPtr<UAnimMontage> FireMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float VerticalRecoilAmount = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float HorizontalRecoilAmount = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float VerticalRecoilRandom = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.0"))
    float HorizontalRecoilRandom = 0.4f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ClampMin = "0.01"))
    float RecoilRecoverySpeed = 8.0f;
};
