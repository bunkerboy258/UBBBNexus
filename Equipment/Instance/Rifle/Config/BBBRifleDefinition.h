#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleHandlingSettings.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleAirborneModifiers.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBRifleDefinition.generated.h"

class UAnimMontage;
class USoundBase;
class UBBBProjectileDefinition;

/** 步枪实例的静态资源与数值配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBRifleDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()

public:
    /** 左手握持目标所使用的装备插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Equip")
    FName LeftHandSocketName = TEXT("LeftHand");

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

    /** 当前步枪发射时使用的本地实体弹丸定义 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Rifle|Fire")
    TObjectPtr<UBBBProjectileDefinition> ProjectileDefinition = nullptr;

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

    /** 腰射的枪口跟随后坐力与摇摆配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|步枪|持枪表现", meta = (DisplayName = "腰射", ToolTip = "角色没有瞄准意图时使用的基础持枪参数"))
    FBBBRifleHandlingSettings HipFireSettings;

    /** 瞄准射击的枪口跟随后坐力与摇摆配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|步枪|持枪表现", meta = (DisplayName = "瞄准射击", ToolTip = "角色具有瞄准意图时使用的基础持枪参数"))
    FBBBRifleHandlingSettings AimFireSettings;

    /** 空中持枪对当前基础配置的修正 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|步枪|持枪表现", meta = (DisplayName = "空中叠加", ToolTip = "腾空时在当前腰射或瞄准配置上乘入这些倍率"))
    FBBBRifleAirborneModifiers AirborneModifiers;

    /** 腰射时贡献给相机的独立配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|步枪|相机后坐力", meta = (DisplayName = "腰射镜头", ToolTip = "由相机蓝图读取武器动画快照后应用 包含独立回零速度"))
    FBBBPlayerCameraRecoilSettings HipFireCameraSettings;

    /** 瞄准射击时贡献给相机的独立配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|步枪|相机后坐力", meta = (DisplayName = "瞄准镜头", ToolTip = "只影响镜头冲击与恢复 不控制角色枪口方向偏移"))
    FBBBPlayerCameraRecoilSettings AimFireCameraSettings;

    /** 空中相机冲击倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|步枪|相机后坐力", meta = (ClampMin = "0.0", DisplayName = "空中镜头冲量倍率", ToolTip = "腾空时乘到当前镜头冲量与随机范围"))
    float AirborneCameraImpulseScale = 1.25f;

    /** 空中相机回零速度倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|步枪|相机后坐力", meta = (ClampMin = "0.01", DisplayName = "空中镜头回零倍率", ToolTip = "腾空时乘到当前镜头回零速度"))
    float AirborneCameraRecoveryScale = 0.8f;
};
