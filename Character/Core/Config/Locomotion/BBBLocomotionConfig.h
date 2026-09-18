#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
#include "BBBLocomotionConfig.generated.h"

class UCurveFloat;

/** 定义官方运动匹配角色的速度、加速、制动和碰撞配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterLocomotionConfig
{
    GENERATED_BODY()

    /** 前进、侧移、后退方向的行走速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Gait")
    FVector WalkSpeeds = FVector(200.0f, 180.0f, 150.0f);

    /** 前进、侧移、后退方向的奔跑速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Gait")
    FVector RunSpeeds = FVector(500.0f, 350.0f, 300.0f);

    /** 前进、侧移、后退方向的冲刺速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Gait")
    FVector SprintSpeeds = FVector(700.0f, 700.0f, 700.0f);

    /** 前进、侧移、后退方向的蹲伏速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Gait")
    FVector CrouchSpeeds = FVector(225.0f, 200.0f, 180.0f);

    /** 将移动方向绝对角映射为前进、侧移、后退插值区间的官方曲线 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Gait")
    TSoftObjectPtr<UCurveFloat> StrafeSpeedMapCurve = TSoftObjectPtr<UCurveFloat>(
        FSoftObjectPath(TEXT("/Game/Blueprints/Data/Curve_StrafeSpeedMap.Curve_StrafeSpeedMap")));

    /** 摇杆输入进入奔跑档位的强度阈值 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Gait", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AnalogRunThreshold = 0.7f;

    /** 侧移模式允许冲刺的最大输入方向偏角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Gait", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float SprintDirectionLimit = 50.0f;

    /** 地面移动最大加速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Acceleration", meta = (ClampMin = "0.0"))
    float MaxAcceleration = 2400.0f;

    /** 地面移动制动减速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Braking", meta = (ClampMin = "0.0"))
    float BrakingDeceleration = 1400.0f;

    /** 地面移动摩擦 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Friction", meta = (ClampMin = "0.0"))
    float GroundFriction = 8.0f;

    /** 制动时使用的独立摩擦 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Friction", meta = (ClampMin = "0.0"))
    float BrakingFriction = 6.0f;

    /** 制动摩擦倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Friction", meta = (ClampMin = "0.0"))
    float BrakingFrictionFactor = 1.0f;

    /** 垂直起跳速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Air", meta = (ClampMin = "0.0"))
    float JumpZVelocity = 500.0f;

    /** 空中控制能力 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Air", meta = (ClampMin = "0.0"))
    float AirControl = 0.25f;

    /** 重力缩放 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Air", meta = (ClampMin = "0.0"))
    float GravityScale = 1.0f;

    /** 最小模拟摇杆移动速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|CMC", meta = (ClampMin = "0.0"))
    float MinAnalogWalkSpeed = 150.0f;

    /** 最大跨越台阶高度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|CMC", meta = (ClampMin = "0.0"))
    float MaxStepHeight = 45.0f;

    /** 最大可行走地面坡度角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|CMC", meta = (ClampMin = "0.0", ClampMax = "90.0"))
    float WalkableFloorAngle = 44.765f;

    /** 制动子步最大时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|CMC", meta = (ClampMin = "0.0166", ClampMax = "0.05"))
    float BrakingSubStepTime = 0.03f;

    /** 站立胶囊半径 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Collision", meta = (ClampMin = "0.0"))
    float CapsuleRadius = 30.0f;

    /** 站立胶囊半高 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Collision", meta = (ClampMin = "0.0"))
    float CapsuleHalfHeight = 86.0f;

    /** 蹲伏胶囊半高 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Collision", meta = (ClampMin = "0.0"))
    float CrouchedHalfHeight = 60.0f;
};
