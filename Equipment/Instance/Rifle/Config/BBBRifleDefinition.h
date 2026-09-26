#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBRifleDefinition.generated.h"

class UAnimMontage;
class USoundBase;

/** 步枪实例的静态资源与数值配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBRifleDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()

public:
    /** 左手握持目标所使用的装备插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Equip")
    FName LeftHandSocketName = TEXT("LeftHand");

    /** 左手握持目标在装备插槽局部空间中的偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Equip")
    FVector LeftHandSocketOffset = FVector::ZeroVector;

    /** 左手握持目标在右手骨骼空间中的额外偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Equip")
    FVector LeftHandIKOffset = FVector::ZeroVector;

    /** 角色装备步枪时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> CharacterEquipMontage = nullptr;

    /** 角色换弹时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> CharacterReloadMontage = nullptr;

    /** 步枪换弹时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> EquipmentReloadMontage = nullptr;

    /** 步枪开火时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Animation")
    TObjectPtr<UAnimMontage> EquipmentFireMontage = nullptr;

    /** 弹匣可容纳的子弹数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "1"))
    int32 AmmoCapacity = 30;

    /** 连续两次开火的最小时间间隔 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire", meta = (ClampMin = "0.01"))
    float FireInterval = 0.2f;

    /** 枪口插槽名称 用于开火位置和声音位置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    FName MuzzleSocketName = TEXT("Muzzle");

    /** 开火时在枪口播放的声音 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    TObjectPtr<USoundBase> FireSound = nullptr;

    /** 每次开火产生的固定垂直后坐量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float VerticalRecoilAmount = 1.0f;

    /** 每次开火产生的固定水平后坐量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float HorizontalRecoilAmount = 0.0f;

    /** 垂直后坐量的随机浮动范围 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    float VerticalRecoilRandom = 0.2f;

    /** 水平后坐量的随机浮动范围 */
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
