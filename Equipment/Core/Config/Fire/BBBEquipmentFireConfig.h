#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentFireConfig.generated.h"

class ABBBBulletActor;
class UAnimMontage;
class USoundBase;

/** 单投射物开火的静态配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentFireConfig
{
    GENERATED_BODY()

    FBBBEquipmentFireConfig();

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
