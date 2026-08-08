
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
        MainHandEquipped.RunSpeed = 360.0f;
        MainHandEquipped.RunAcceleration = 1200.0f;
    }

    /** 空手移动参数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion")
    FBBBCharacterLocomotionProfileConfig Unarmed;

    /** 主手持有装备时的移动参数 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion")
    FBBBCharacterLocomotionProfileConfig MainHandEquipped;

    /** 垂直起跳速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Jump", meta = (ClampMin = "0.0"))
    float JumpZVelocity = 420.0f;
};
