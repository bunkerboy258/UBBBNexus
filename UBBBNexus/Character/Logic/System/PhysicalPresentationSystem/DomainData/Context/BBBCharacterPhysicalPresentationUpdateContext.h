#pragma once
class ABBBCharacter;
class USkeletalMeshComponent;
class UPhysicalAnimationComponent;
class UBBBCharacterHitReactionComponent;
struct FBBBCharacterRuntimeData;

/** 物理表现请求阶段的栈上依赖 */
struct FBBBCharacterPhysicalPresentationUpdateContext final
{
    /** 角色唯一载体 */
    ABBBCharacter &Character;
    /** 唯一黑板 */
    FBBBCharacterRuntimeData &Data;
    /** 实际角色网格 */
    USkeletalMeshComponent &Mesh;
    /** 插件局部受击组件 */
    UBBBCharacterHitReactionComponent &HitReaction;
    /** 局部姿势恢复组件 */
    UPhysicalAnimationComponent &PhysicalAnimation;
};
