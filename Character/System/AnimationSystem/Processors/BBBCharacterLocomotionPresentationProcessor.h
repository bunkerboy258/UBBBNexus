
#pragma once
#include "CoreMinimal.h"
class UCharacterMovementComponent;
struct FBBBAimRuntimeData;
struct FBBBCharacterAnimationState;
struct FBBBCharacterLocomotionConfig;
struct FBBBIntentRuntimeData;

//推导移动状态枚举
class FBBBCharacterLocomotionPresentationProcessor final
{
public:

    /**
     * 根据移动与瞄准状态推导移动状态枚举并提交移动输入
     * @param Movement	角色移动组件
     * @param Config	Locomotion配置
     * @param AimData	瞄准运行时数据
     * @param IntentData	意图运行时数据
     * @param AnimationState	输出的动画状态
     */
    void Update(
        const UCharacterMovementComponent &Movement,
        const FBBBCharacterLocomotionConfig &Config,
        const FBBBAimRuntimeData &AimData,
        const FBBBIntentRuntimeData &IntentData,
        FBBBCharacterAnimationState &AnimationState) const;
};
