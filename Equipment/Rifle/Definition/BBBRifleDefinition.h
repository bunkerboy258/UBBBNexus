#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Definition/BBBEquipmentDefinition.h"
#include "BBBRifleDefinition.generated.h"

class ABBBBulletActor;
class UAnimMontage;
class USoundBase;

/** 步枪实例的静态资源与数值配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBRifleDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Equip")
    FName LeftHandSocketName = TEXT("LeftHand");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Equip")
    FVector LeftHandSocketOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Equip")
    FVector LeftHandIKOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> FireMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> ReloadMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "1"))
    int32 AmmoCapacity = 30;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "0.01"))
    float FireInterval = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    TSubclassOf<ABBBBulletActor> BulletActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "0.01"))
    float MuzzleSpeed = 30000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    FName MuzzleSocketName = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    TObjectPtr<USoundBase> FireSound = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float VerticalRecoilAmount = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float HorizontalRecoilAmount = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float VerticalRecoilRandom = 0.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float HorizontalRecoilRandom = 0.4f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "0.01"))
    float RecoilRecoverySpeed = 8.0f;
};
