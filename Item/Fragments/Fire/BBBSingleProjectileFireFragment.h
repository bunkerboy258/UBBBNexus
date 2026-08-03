
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/Base/BBBFireFragment.h"
#include "BBBSingleProjectileFireFragment.generated.h"
class ABBBBulletActor;
class ABBBWeaponActor;
class UAnimMontage;
class UBBBFireRuntimeData;
class UBBBItemFragmentRuntimeData;
class USoundBase;
struct FBBBShotEvent;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBSingleProjectileFireFragment final : public UBBBFireFragment
{
    GENERATED_BODY()
public:

    UBBBSingleProjectileFireFragment();

    virtual bool Fire(ABBBWeaponActor &Weapon, UBBBFireRuntimeData &RuntimeData, FBBBShotEvent &OutShotEvent) const override;

    virtual void Present(ABBBWeaponActor &Weapon) const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon", meta = (ClampMin = "0.01"))
    float FireInterval = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Projectile")
    TSubclassOf<ABBBBulletActor> BulletActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Projectile", meta = (ClampMin = "0.01"))
    float MuzzleSpeed = 30000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Socket")
    FName MuzzleSocketName = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation")
    TObjectPtr<UAnimMontage> FireMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Audio")
    TObjectPtr<USoundBase> FireSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float VerticalRecoilAmount = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float HorizontalRecoilAmount = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float VerticalRecoilRandom = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.0"))
    float HorizontalRecoilRandom = 0.4f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Weapon|Recoil", meta = (ClampMin = "0.01"))
    float RecoilRecoverySpeed = 8.0f;
};
