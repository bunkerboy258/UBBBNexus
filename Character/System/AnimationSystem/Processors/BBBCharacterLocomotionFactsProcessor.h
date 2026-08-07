#pragma once

#include "CoreMinimal.h"

class UCharacterMovementComponent;
struct FBBBAimRuntimeData;
struct FBBBCharacterAnimationState;
struct FBBBCharacterEquipmentState;
struct FBBBFacingRuntimeData;

/** 整理角色移动事实供动画蓝图自行决定表现 */
class FBBBCharacterLocomotionFactsProcessor final
{
public:
    /**
     * 从角色领域状态与实际速度生成动画移动事实
     * @param Movement		角色移动组件
     * @param AimData		瞄准运行数据
     * @param FacingData		朝向运行数据
     * @param EquipmentState	装备状态
     * @param AnimationState	动画事实输出
     */
    void Update(
        const UCharacterMovementComponent &Movement,
        const FBBBAimRuntimeData &AimData,
        const FBBBFacingRuntimeData &FacingData,
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterAnimationState &AnimationState) const;
};
