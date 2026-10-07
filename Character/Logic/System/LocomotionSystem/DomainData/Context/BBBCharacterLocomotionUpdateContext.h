#pragma once

class ACharacter;
class UCharacterMovementComponent;
class UCurveFloat;
struct FBBBCharacterControlState;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterLocomotionState;

class UMotionWarpingComponent;
struct FBBBTraversalConfig;
struct FBBBCharacterTraversalState;
struct FBBBCharacterWorldState;
struct FBBBCharacterNetworkIdentityState;
struct FBBBCharacterAnimationFactState;
struct FBBBCharacterAnimationMontageState;

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
    /** 攀爬系统已经成立的动作与交接结果 */
    const FBBBCharacterTraversalState &Traversal;
    /** 主管线已经裁决的执行模式 */
    const FBBBCharacterNetworkIdentityState &Execution;
    /** 上次动画更新发布的播放事实 */
    const FBBBCharacterAnimationFactState &AnimationFacts;
};
