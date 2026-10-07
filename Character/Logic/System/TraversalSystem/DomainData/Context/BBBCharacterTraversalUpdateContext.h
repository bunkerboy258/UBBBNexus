#pragma once

class ABBBCharacter;
class UCharacterMovementComponent;
class UMotionWarpingComponent;
struct FBBBCharacterTraversalState;
struct FBBBCharacterControlState;
struct FBBBTraversalConfig;
struct FBBBCharacterWorldState;
struct FBBBCharacterNetworkIdentityState;
struct FBBBCharacterAnimationMontageState;
struct FBBBCharacterLocomotionState;
struct FBBBCharacterTraversalAnimationState;

/** CMC 执行前生成攀爬结果的栈上上下文 */
struct FBBBCharacterTraversalUpdateContext final
{
    /** 动作所属角色 */
    ABBBCharacter &Character;
    /** 只读检测胶囊可通行性与地面支撑 */
    const UCharacterMovementComponent &Movement;
    /** 本系统维护的翻越状态 */
    FBBBCharacterTraversalState &Traversal;
    /** 输入系统已经裁决的控制状态 */
    const FBBBCharacterControlState &ControlState;
    /** 独立翻越配置 */
    const FBBBTraversalConfig &TraversalConfig;
    /** 主管线世界时间 */
    const FBBBCharacterWorldState &World;
    /** 主管线已经裁决的执行模式 */
    const FBBBCharacterNetworkIdentityState &Execution;
    /** 本帧输入批准的全身播放请求 */
    const FBBBCharacterAnimationMontageState &Montages;
    /** CMC 写入方发布的控制交接事实 */
    const FBBBCharacterLocomotionState &Locomotion;
    /** 动画所属动作的实际播放与退出结果 */
    const FBBBCharacterTraversalAnimationState &Playback;
    /** 官方根运动校正组件 */
    UMotionWarpingComponent &Warping;
};
