#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBLinkedAnimInstance.generated.h"

class UBBBAnimInstance;
class UBBBEquipmentInstance;

/** 链接动画层访问主动画实例的线程安全基类 */
UCLASS(Abstract)
class ABBB_EVAC_API UBBBLinkedAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    /** @return 当前骨骼网格体上的 BBB 主动画实例 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    UBBBAnimInstance *GetBBBMainAnimInstanceThreadSafe() const;

    /** @return 链接层读取的当前步态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    EBBBCharacterGait GetLinkedGait() const;

    /** @return 链接层读取的行走状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsLinkedWalking() const;

    /** @return 链接层读取的奔跑状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsLinkedRunning() const;

    /** @return 链接层读取的冲刺状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsLinkedSprinting() const;

    /** @return 链接层读取的瞄准状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool IsLinkedAiming() const;

    /** @return 链接层读取的瞄准意图权重 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetLinkedAimIntentAlpha() const;

    /** @return 链接层读取的瞄准 IK 权重 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    float GetLinkedAimIKAlpha() const;

    /** @return 链接层读取的组件空间瞄准目标 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FVector GetLinkedAimTargetComponentSpace() const;

    /** @return 链接层读取的瞄准来源本地变换 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FTransform GetLinkedAimSourceLocalTransform() const;

    /** @return 链接层读取的瞄准目标有效状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasLinkedValidAimTarget() const;

    /** @return 链接层读取的瞄准来源有效状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasLinkedValidAimSource() const;

    /** @return 链接层读取的主手装备状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool HasLinkedMainHandEquipment() const;

    /** @return 链接层读取的主手装备实例 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    UBBBEquipmentInstance *GetLinkedMainHandEquipmentInstance() const;

    /** @return 链接层读取的换弹状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool IsLinkedReloading() const;

    /** @return 链接层读取的上次开火时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    float GetLinkedTimeSinceLastFire() const;

    /**
     * 判断链接层读取的最近一次开火是否仍应维持持枪姿势
     * @param Duration\t持枪姿势维持时长
     * @return 是否仍处于指定维持时间内
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool ShouldLinkedRaiseWeaponAfterFiring(float Duration) const;

    /** @return 链接层读取的左手 IK 偏移是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool HasLinkedLeftHandIKOffsetRightHand() const;

    /** @return 链接层读取的相对右手骨骼的左手 IK 偏移 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FVector GetLinkedLeftHandIKOffsetRightHand() const;

    /** @return 链接层读取的角色世界位置 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    FVector GetLinkedSourceActorLocation() const;

    /** @return 链接层读取的角色世界旋转 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    FRotator GetLinkedSourceActorRotation() const;

    /** @return 链接层读取的角色世界速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    FVector GetLinkedSourceVelocity() const;

    /** @return 链接层读取的上次更新世界速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    FVector GetLinkedSourceLastUpdateVelocity() const;

    /** @return 链接层读取的角色世界加速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    FVector GetLinkedSourceAcceleration() const;

    /** @return 链接层读取的地面摩擦力 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    float GetLinkedSourceGroundFriction() const;

    /** @return 链接层读取的制动摩擦力 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    float GetLinkedSourceBrakingFriction() const;

    /** @return 链接层读取的制动摩擦系数 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    float GetLinkedSourceBrakingFrictionFactor() const;

    /** @return 链接层读取的行走制动减速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    float GetLinkedSourceBrakingDecelerationWalking() const;

    /** @return 链接层读取的独立制动摩擦力开关 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    bool HasLinkedSeparateBrakingFriction() const;

    /** @return 链接层读取的重力加速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    float GetLinkedSourceGravityZ() const;

    /** @return 链接层读取的地面移动状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    bool IsLinkedMovingOnGround() const;

    /** @return 链接层读取的蹲伏状态 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    bool IsLinkedCrouching() const;

    /** @return 链接层读取的胶囊体底部到地面距离 */
    UFUNCTION(BlueprintPure, Category = "BBB|Animation Facts", meta = (BlueprintThreadSafe))
    float GetLinkedGroundDistance() const;
};
