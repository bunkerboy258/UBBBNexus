#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterPhysicalPresentationProcessor.h"
class ABBBCharacter;
class USkeletalMeshComponent;
class UPhysicalAnimationComponent;
class UBBBCharacterHitReactionComponent;
struct FBBBCharacterRuntimeData;

/** 只装配并调度角色物理表现 */
class FBBBCharacterPhysicalPresentationSystem final
{
  private:
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterUpdatePipeline;

    /**
     * @param InCharacter		角色载体
     * @param InData			唯一聚合黑板
     * @param InMesh			实际角色网格
     * @param InReaction		局部受击组件
     * @param InPhysicalAnimation	局部姿势恢复组件
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter, FBBBCharacterRuntimeData &InData, USkeletalMeshComponent &InMesh,
                    UBBBCharacterHitReactionComponent &InReaction, UPhysicalAnimationComponent &InPhysicalAnimation);
    /** @return 无 */
    void Update() const;

    /** 角色唯一载体 */
    ABBBCharacter *Character = nullptr;
    /** 聚合黑板 */
    FBBBCharacterRuntimeData *Data = nullptr;
    /** 角色网格 */
    USkeletalMeshComponent *Mesh = nullptr;
    /** 局部受击组件 */
    UBBBCharacterHitReactionComponent *Reaction = nullptr;
    /** 物理动画组件 */
    UPhysicalAnimationComponent *PhysicalAnimation = nullptr;
    /** 固定调度的表现处理器 */
    FBBBCharacterPhysicalPresentationProcessor Processor;
};
