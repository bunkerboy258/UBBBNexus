#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
#include "BBBLocomotionConfig.generated.h"

class UCurveFloat;

/** 定义官方运动匹配角色的速度 加速 制动和碰撞配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterLocomotionConfig
{
    GENERATED_BODY()

    /** 前进 侧移 后退方向的行走速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|步态", meta = (DisplayName = "行走速度"))
    FVector WalkSpeeds = FVector(200.0f, 180.0f, 150.0f);

    /** 前进 侧移 后退方向的奔跑速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|步态", meta = (DisplayName = "奔跑速度"))
    FVector RunSpeeds = FVector(500.0f, 350.0f, 300.0f);

    /** 前进 侧移 后退方向的蹲伏速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|步态", meta = (DisplayName = "蹲伏速度"))
    FVector CrouchSpeeds = FVector(225.0f, 200.0f, 180.0f);

    /** 将移动方向绝对角映射为前进 侧移 后退插值区间的官方曲线 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|步态", meta = (DisplayName = "侧移速度映射曲线"))
    TSoftObjectPtr<UCurveFloat> StrafeSpeedMapCurve = TSoftObjectPtr<UCurveFloat>(
        FSoftObjectPath(TEXT("/Game/_ThirdParty/Environment/ElectricDreams/Blueprints/Data/Curve_StrafeSpeedMap.Curve_StrafeSpeedMap")));

    /** 摇杆输入进入跑步档位的强度阈值 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|步态", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "模拟输入奔跑阈值"))
    float AnalogRunThreshold = 0.7f;

    /** 允许跑步的最大输入方向偏角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|步态", meta = (ClampMin = "0.0", ClampMax = "180.0", DisplayName = "奔跑方向限制"))
    float RunDirectionLimit = 50.0f;

    /** 地面移动最大加速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|加速度", meta = (ClampMin = "0.0", DisplayName = "最大加速度"))
    float MaxAcceleration = 2400.0f;

    /** 地面移动制动减速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|制动", meta = (ClampMin = "0.0", DisplayName = "制动减速度"))
    float BrakingDeceleration = 1400.0f;

    /** 地面移动摩擦 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|摩擦力", meta = (ClampMin = "0.0", DisplayName = "地面摩擦力"))
    float GroundFriction = 8.0f;

    /** 制动时使用的独立摩擦 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|摩擦力", meta = (ClampMin = "0.0", DisplayName = "制动摩擦力"))
    float BrakingFriction = 6.0f;

    /** 制动摩擦倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|摩擦力", meta = (ClampMin = "0.0", DisplayName = "制动摩擦系数"))
    float BrakingFrictionFactor = 1.0f;

    /** 垂直起跳速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|空中", meta = (ClampMin = "0.0", DisplayName = "跳跃垂直速度"))
    float JumpZVelocity = 500.0f;

    /** 空中控制能力 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|空中", meta = (ClampMin = "0.0", DisplayName = "空中控制系数"))
    float AirControl = 0.25f;

    /** 重力缩放 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|空中", meta = (ClampMin = "0.0", DisplayName = "重力倍率"))
    float GravityScale = 1.0f;

    /** 最小模拟摇杆移动速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|角色移动组件", meta = (ClampMin = "0.0", DisplayName = "最小模拟输入行走速度"))
    float MinAnalogWalkSpeed = 150.0f;

    /** 最大跨越台阶高度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|角色移动组件", meta = (ClampMin = "0.0", DisplayName = "最大台阶高度"))
    float MaxStepHeight = 45.0f;

    /** 最大可行走地面坡度角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|角色移动组件", meta = (ClampMin = "0.0", ClampMax = "90.0", DisplayName = "可行走坡面角度"))
    float WalkableFloorAngle = 44.765f;

    /** 制动子步最大时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|角色移动组件", meta = (ClampMin = "0.0166", ClampMax = "0.05", DisplayName = "制动子步长"))
    float BrakingSubStepTime = 0.03f;

    /** 站立胶囊半径 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|碰撞", meta = (ClampMin = "0.0", DisplayName = "胶囊体半径"))
    float CapsuleRadius = 30.0f;

    /** 站立胶囊半高 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|碰撞", meta = (ClampMin = "0.0", DisplayName = "胶囊体半高"))
    float CapsuleHalfHeight = 86.0f;

    /** 蹲伏胶囊半高 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|碰撞", meta = (ClampMin = "0.0", DisplayName = "蹲伏胶囊体半高"))
    float CrouchedHalfHeight = 60.0f;

    /** 倒地四向与斜向的统一速度上限 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|倒地", meta = (DisplayName = "倒地移动速度", ClampMin = "0"))
    float DownedSpeed = 60.0f;

    /** 倒地身体最大转向速度 单位度每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|倒地", meta = (DisplayName = "倒地转向速度", ClampMin = "0"))
    float DownedTurnSpeed = 90.0f;

    /** 跪姿独立碰撞胶囊半径 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|倒地", meta = (DisplayName = "倒地胶囊半径", ClampMin = "1"))
    float DownedCapsuleRadius = 32.0f;

    /** 跪姿独立碰撞胶囊半高 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|移动|倒地", meta = (DisplayName = "倒地胶囊半高", ClampMin = "1"))
    float DownedCapsuleHalfHeight = 40.0f;
};
