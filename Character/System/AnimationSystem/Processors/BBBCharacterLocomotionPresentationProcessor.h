
#pragma once
#include "CoreMinimal.h"
class UCharacterMovementComponent;
struct FBBBAimRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationState;
struct FBBBCharacterLocomotionConfig;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;

//推导移动状态枚举
class FBBBCharacterLocomotionPresentationProcessor final
{
public:

    /**
     * 根据移动与瞄准状态推导移动状态枚举并提交移动输入与原地转身状态
     * @param Movement	角色移动组件
     * @param Config	Locomotion配置
     * @param AimData	瞄准运行时数据
     * @param FacingData	朝向运行时数据
     * @param IntentData	意图运行时数据
     * @param AnimationData	动画运行时数据
     * @param AnimationState	输出的动画状态
     */
    void Update(
        const UCharacterMovementComponent &Movement,
        const FBBBCharacterLocomotionConfig &Config,
        const FBBBAimRuntimeData &AimData,
        const FBBBFacingRuntimeData &FacingData,
        const FBBBIntentRuntimeData &IntentData,
        FBBBAnimationRuntimeData &AnimationData,
        FBBBCharacterAnimationState &AnimationState) const;
};
