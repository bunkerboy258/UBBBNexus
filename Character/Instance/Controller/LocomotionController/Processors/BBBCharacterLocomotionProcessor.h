#pragma once

#include "CoreMinimal.h"

class ACharacter;
class UCharacterMovementComponent;
class UCurveFloat;
struct FBBBCharacterControlState;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterLocomotionRuntimeData;

/** 根据控制黑板驱动角色移动组件 */
class FBBBCharacterLocomotionProcessor final
{
public:
    void Update(
        ACharacter &Character,
        UCharacterMovementComponent &Movement,
        FBBBCharacterLocomotionRuntimeData &RuntimeData,
        const FBBBCharacterControlState &ControlData,
        const FBBBCharacterLocomotionConfig &Config,
        const UCurveFloat &StrafeSpeedMapCurve) const;
};
