#pragma once

class ABBBCharacter;
class UBBBAnimInstance;
class USkeletalMeshComponent;
struct FBBBCharacterAnimationConfig;
struct FBBBCharacterAnimationFactState;
struct FBBBCharacterAnimationLayerState;
struct FBBBCharacterAnimationMontageState;
struct FBBBCharacterEquipmentSelectionState;
struct FBBBCharacterRuntimeData;
struct FBBBCharacterWorldState;

/** 本次角色动画更新使用的栈上上下文 */
struct FBBBCharacterAnimationUpdateContext final
{
    /** 动画所属角色 */
    ABBBCharacter &Character;

    /** 角色完整运行时黑板 */
    FBBBCharacterRuntimeData &RuntimeData;

    /** 角色骨骼网格 */
    USkeletalMeshComponent &CharacterMesh;

    /** 角色动画实例 */
    UBBBAnimInstance &AnimationInstance;

    /** 角色动画事实状态 */
    FBBBCharacterAnimationFactState &AnimationFactState;

    /** 角色动画层状态 */
    FBBBCharacterAnimationLayerState &AnimationLayerState;

    /** 角色动画蒙太奇请求状态 */
    FBBBCharacterAnimationMontageState &AnimationMontageState;

    /** 角色装备选择状态 */
    const FBBBCharacterEquipmentSelectionState &EquipmentSelectionState;

    /** 当前帧世界快照 */
    const FBBBCharacterWorldState &WorldState;

    /** 角色动画配置 */
    const FBBBCharacterAnimationConfig &AnimationConfig;
};
