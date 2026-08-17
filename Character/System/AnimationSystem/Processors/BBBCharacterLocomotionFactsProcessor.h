#pragma once

#include "CoreMinimal.h"

class UCharacterMovementComponent;
struct FBBBCharacterAnimationConfig;
struct FBBBCharacterAnimationState;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterEquipmentState;
struct FBBBAimRuntimeData;
struct FBBBAnimationRuntimeData;

/** 整理角色移动事实供动画蓝图自行决定表现 */
class FBBBCharacterLocomotionFactsProcessor final
{
public:
    /**
     * 从角色领域状态与实际速度生成动画移动事实
     * @param Movement		角色移动组件
     * @param AimData		角色瞄准数据
     * @param EquipmentState	装备状态
     * @param LocomotionConfig	角色移动配置
     * @param AnimationState	动画事实输出
     */
    void Update(
        const UCharacterMovementComponent &Movement,
        const FBBBAimRuntimeData &AimData,
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBCharacterLocomotionConfig &LocomotionConfig,
        const FBBBCharacterAnimationConfig &AnimationConfig,
        float DeltaSeconds,
        FBBBAnimationRuntimeData &AnimationData,
        FBBBCharacterAnimationState &AnimationState) const;
};
