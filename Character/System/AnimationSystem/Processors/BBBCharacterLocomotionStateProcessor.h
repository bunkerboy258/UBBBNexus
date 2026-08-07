
#pragma once
#include "CoreMinimal.h"
class UCharacterMovementComponent;
struct FBBBAimRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationState;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterLocomotionConfig;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;

//推导空手与步枪移动状态枚举
class FBBBCharacterLocomotionStateProcessor final
{
public:

    /**
     * 根据装备姿态、速度档位与移动状态推导动画枚举
     * @param Movement	角色移动组件
     * @param DeltaSeconds	当前帧时间间隔
     * @param Config	Locomotion配置
     * @param AimData	瞄准运行时数据
     * @param FacingData	朝向运行时数据
     * @param IntentData	意图运行时数据
     * @param EquipmentState	角色当前装备状态
     * @param AnimationData	动画运行时数据
     * @param AnimationState	输出的动画状态
     */
    void Update(
        const UCharacterMovementComponent &Movement,
        float DeltaSeconds,
        const FBBBCharacterLocomotionConfig &Config,
        const FBBBAimRuntimeData &AimData,
        const FBBBFacingRuntimeData &FacingData,
        const FBBBIntentRuntimeData &IntentData,
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBAnimationRuntimeData &AnimationData,
        FBBBCharacterAnimationState &AnimationState) const;
};

