#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterAnimationConfig.generated.h"

class UAnimInstance;
class UAnimMontage;

/** 武器动作动画配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterWeaponAnimationConfig
{
    GENERATED_BODY()

    /** 装备蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Weapon")
    TObjectPtr<UAnimMontage> EquipMontage;

    /** 换弹蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Weapon")
    TObjectPtr<UAnimMontage> ReloadMontage;

    /** 开火蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Weapon")
    TObjectPtr<UAnimMontage> FireMontage;

};

/**
 * 配置动画系统从角色实际运动中识别表现事实所需的阈值
 */
USTRUCT(BlueprintType)
struct FBBBCharacterAnimationConfig
{
    GENERATED_BODY()

    /** 未装备专用动画层时链接的默认动画层 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Layer")
    TSubclassOf<UAnimInstance> DefaultAnimationLayerClass;

    /** 当前角色完整武器动作动画组 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Weapon")
    FBBBCharacterWeaponAnimationConfig Weapon;

    /** 角色实际水平转速超过该值时生成对应方向的原地转身信号，单位为度每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Facing", meta = (ClampMin = "0.0"))
    float TurnSignalRateThreshold = 20.0f;

    /** 将角色离散旋转量转换为稳定动画转速所使用的低通时间，单位为秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Facing", meta = (ClampMin = "0.001"))
    float TurnRateSmoothingTime = 0.12f;

    /** 动画转速每秒允许变化的最大幅度，单位为度每平方秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Facing", meta = (ClampMin = "0.0"))
    float MaxTurnRateChangeSpeed = 720.0f;
};
