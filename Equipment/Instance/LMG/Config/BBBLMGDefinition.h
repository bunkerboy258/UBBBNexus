#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Config/Handling/BBBLMGHandlingSettings.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Config/Handling/BBBLMGAirborneModifiers.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBLMGDefinition.generated.h"

class UAnimMontage;
class UAnimSequence;
class UBBBProjectileDefinition;

/** 轻机枪实例的静态资源与数值配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBLMGDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()

public:
    /** @return 无 为轻机枪配置建立明确类型 */
    UBBBLMGDefinition()
    {
        EquipmentType = EBBBEquipmentType::LMG;
    }

    /** 固定开火方式 为真时持续请求可连续射击 为假时每次按下只射击一次 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|开火", meta = (DisplayName = "自动射击", ToolTip = "型号固定配置 不提供玩家射击模式切换"))
    bool bAutomaticFire = true;

    /** 左手握持目标所使用的装备插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|装备", meta = (DisplayName = "左手插槽名"))
    FName LeftHandSocketName = TEXT("LeftHand");

    /** 左手握持目标在右手骨骼空间中的额外偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|装备", meta = (DisplayName = "左手 IK 偏移"))
    FVector LeftHandIKOffset = FVector::ZeroVector;

    /** 左手握持目标相对装备插槽局部坐标的额外旋转 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|装备", meta = (DisplayName = "左手 IK 旋转"))
    FRotator LeftHandIKRotation = FRotator::ZeroRotator;

    /** 角色装备轻机枪时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|动画", meta = (DisplayName = "角色装备蒙太奇"))
    TObjectPtr<UAnimMontage> CharacterEquipMontage = nullptr;

    /** 角色换弹时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|动画", meta = (DisplayName = "角色换弹蒙太奇"))
    TObjectPtr<UAnimMontage> CharacterReloadMontage = nullptr;

    /** 本武器独立持有的角色局部空间加法后坐力序列 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|动画", meta = (DisplayName = "角色后坐力动画", ToolTip = "开火后按距上次开火时间求值 只控制后向震动 权重由持枪表现配置决定"))
    TObjectPtr<UAnimSequence> CharacterRecoilAnimation = nullptr;

    /** 轻机枪换弹时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|动画", meta = (DisplayName = "装备换弹蒙太奇"))
    TObjectPtr<UAnimMontage> EquipmentReloadMontage = nullptr;

    /** 轻机枪开火时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|动画", meta = (DisplayName = "装备开火蒙太奇"))
    TObjectPtr<UAnimMontage> EquipmentFireMontage = nullptr;

    /** 当前轻机枪发射时使用的本地实体弹丸定义 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|开火", meta = (DisplayName = "弹丸定义"))
    TObjectPtr<UBBBProjectileDefinition> ProjectileDefinition = nullptr;

    /** 弹匣可容纳的子弹数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|开火", meta = (ClampMin = "1", DisplayName = "弹药容量"))
    int32 AmmoCapacity = 30;

    /** 连续两次开火的最小时间间隔 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|开火", meta = (ClampMin = "0.01", DisplayName = "开火间隔"))
    float FireInterval = 0.2f;

    /** 枪口插槽名称 用于确定发射物生成位置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|开火", meta = (DisplayName = "枪口插槽名"))
    FName MuzzleSocketName = TEXT("Muzzle");

    /** 腰射的枪口跟随后坐力与摇摆配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|持枪表现", meta = (DisplayName = "腰射", ToolTip = "角色没有瞄准意图时使用的基础持枪参数"))
    FBBBLMGHandlingSettings HipFireSettings;

    /** 瞄准射击的枪口跟随后坐力与摇摆配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|持枪表现", meta = (DisplayName = "瞄准射击", ToolTip = "角色具有瞄准意图时使用的基础持枪参数"))
    FBBBLMGHandlingSettings AimFireSettings;

    /** 空中持枪对当前基础配置的修正 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|持枪表现", meta = (DisplayName = "空中叠加", ToolTip = "腾空时在当前腰射或瞄准配置上乘入这些倍率"))
    FBBBLMGAirborneModifiers AirborneModifiers;

    /** 腰射时贡献给相机的独立配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|相机后坐力", meta = (DisplayName = "腰射镜头", ToolTip = "由相机蓝图读取武器动画快照后应用 包含独立回零速度"))
    FBBBPlayerCameraRecoilSettings HipFireCameraSettings;

    /** 瞄准射击时贡献给相机的独立配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|相机后坐力", meta = (DisplayName = "瞄准镜头", ToolTip = "只影响镜头冲击与恢复 不控制角色枪口方向偏移"))
    FBBBPlayerCameraRecoilSettings AimFireCameraSettings;

    /** 空中相机冲击倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|相机后坐力", meta = (ClampMin = "0.0", DisplayName = "空中镜头冲量倍率", ToolTip = "腾空时乘到当前镜头冲量与随机范围"))
    float AirborneCameraImpulseScale = 1.25f;

    /** 空中相机回零速度倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|轻机枪|相机后坐力", meta = (ClampMin = "0.01", DisplayName = "空中镜头回零倍率", ToolTip = "腾空时乘到当前镜头回零速度"))
    float AirborneCameraRecoveryScale = 0.8f;
};
