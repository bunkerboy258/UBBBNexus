#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentDefinition.h"
#include "BBBRifleDefinition.generated.h"

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
    TObjectPtr<UAnimMontage> CharacterEquipMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> CharacterReloadMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> EquipmentReloadMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> EquipmentFireMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "1"))
    int32 AmmoCapacity = 30;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "0.01"))
    float FireInterval = 0.2f;

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

    /** 每次开火贡献的镜头倾斜角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float CameraRollImpulseDegrees = 0.0f;

    /** 每次开火贡献的角色额外向上瞄准角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float AimPitchImpulseDegrees = 0.0f;

    /** 每次开火贡献的角色额外向右瞄准角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float AimYawImpulseDegrees = 0.0f;
};
