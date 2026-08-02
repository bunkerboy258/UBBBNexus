
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

    void Update(
        const UCharacterMovementComponent &Movement,
        const FBBBCharacterLocomotionConfig &Config,
        const FBBBAimRuntimeData &AimData,
        const FBBBIntentRuntimeData &IntentData,
        FBBBCharacterAnimationState &AnimationState) const;
};
