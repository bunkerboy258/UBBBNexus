
#pragma once
#include "CoreMinimal.h"
#include "BBBLocomotionConfig.generated.h"

/** 定义一种装备状态下的走跑移动参数 */
USTRUCT(BlueprintType)
struct FBBBCharacterLocomotionProfileConfig
{
    GENERATED_BODY()

    /** 慢速移动的最大速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion", meta = (ClampMin = "0.0"))
    float WalkSpeed = 280.0f;

    /** 慢速移动的加速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion", meta = (ClampMin = "0.0"))
    float WalkAcceleration = 800.0f;

    /** 快速移动的最大速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion", meta = (ClampMin = "0.0"))
    float RunSpeed = 650.0f;

    /** 快速移动的加速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion", meta = (ClampMin = "0.0"))
    float RunAcceleration = 1600.0f;
};

/** 定义空手、主手持有装备与跳跃移动参数 */
USTRUCT(BlueprintType)
struct FBBBCharacterLocomotionConfig
{
    GENERATED_BODY()

    FBBBCharacterLocomotionConfig()
    {
        Unarmed.WalkSpeed = 250.0f;
        Unarmed.WalkAcceleration = 800.0f;
        Unarmed.RunSpeed = 500.0f;
        Unarmed.RunAcceleration = 1600.0f;

        MainHandEquipped.WalkSpeed = 200.0f;
        MainHandEquipped.WalkAcceleration = 650.0f;
        MainHandEquipped.RunSpeed = 450.0f;
        MainHandEquipped.RunAcceleration = 1200.0f;

        Strafe.WalkSpeed = 150.0f;
        Strafe.WalkAcceleration = 650.0f;
        Strafe.RunSpeed = 300.0f;
        Strafe.RunAcceleration = 1200.0f;
    }

    /** 空手移动参数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion")
    FBBBCharacterLocomotionProfileConfig Unarmed;

    /** 主手持有装备时的移动参数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion")
    FBBBCharacterLocomotionProfileConfig MainHandEquipped;

    /** 瞄准横移参数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion")
    FBBBCharacterLocomotionProfileConfig Strafe;

    /** 垂直起跳速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Jump", meta = (ClampMin = "0.0"))
    float JumpZVelocity = 420.0f;
};
