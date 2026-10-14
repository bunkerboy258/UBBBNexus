#pragma once
#include "CoreMinimal.h"
class UPhysicalMaterial;
#include "BBBThrowablePhysicsConfig.generated.h"

/** 投掷物Physics静态配置 */
USTRUCT(BlueprintType)
struct FBBBThrowablePhysicsConfig final
{
    GENERATED_BODY()

    /** 物理球体半径 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "碰撞半径", ToolTip = "物理球体半径 单位厘米", ClampMin = "0.5", ForceUnits = "cm"))
    float Radius = 4.0f;

    /** 物理球体质量 单位千克 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "质量", ToolTip = "物理球体质量 单位千克", ClampMin = "0.01", ForceUnits = "kg"))
    float MassKg = 0.4f;

    /** 速度衰减强度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "线性阻尼", ToolTip = "速度衰减强度", ClampMin = "0"))
    float LinearDamping = 0.1f;

    /** 旋转速度衰减强度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "角阻尼", ToolTip = "旋转速度衰减强度", ClampMin = "0"))
    float AngularDamping = 0.4f;

    /** 使用世界重力 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "启用重力", ToolTip = "使用世界重力"))
    bool bEnableGravity = true;

    /** 高速飞行时使用连续碰撞检测 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "连续碰撞", ToolTip = "高速飞行时使用连续碰撞检测"))
    bool bCCD = true;

    /** 物理球体使用的碰撞预设 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "碰撞预设", ToolTip = "物理球体使用的碰撞预设"))
    FName CollisionProfile = TEXT("PhysicsActor");

    /** 是否与角色胶囊产生物理碰撞 不控制爆炸伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "碰撞角色", ToolTip = "是否与角色胶囊产生物理碰撞 不控制爆炸伤害"))
    bool bCollideWithPawns = false;

    /** 配置摩擦和弹性 决定弹跳与滚动 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "物理材质", ToolTip = "配置摩擦和弹性 决定弹跳与滚动"))
    TObjectPtr<UPhysicalMaterial> Material = nullptr;
};
