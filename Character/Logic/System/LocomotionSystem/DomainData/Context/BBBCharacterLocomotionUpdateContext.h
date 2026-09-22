#pragma once

class ACharacter;
class UCharacterMovementComponent;
class UCurveFloat;
struct FBBBCharacterControlState;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterLocomotionState;

/** 本次角色移动更新使用的栈上上下文 */
struct FBBBCharacterLocomotionUpdateContext final
{
    /** 受控角色 */
    ACharacter &Character;

    /** 角色移动组件 */
    UCharacterMovementComponent &Movement;

    /** 角色移动状态 */
    FBBBCharacterLocomotionState &LocomotionState;

    /** 输入系统已经裁决的控制状态 */
    const FBBBCharacterControlState &ControlState;

    /** 角色移动配置 */
    const FBBBCharacterLocomotionConfig &Config;

    /** 横移方向速度曲线 */
    const UCurveFloat &StrafeSpeedMapCurve;
};
