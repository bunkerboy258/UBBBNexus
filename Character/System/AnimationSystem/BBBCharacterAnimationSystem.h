#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAimPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterEquipmentPoseProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionFactsProcessor.h"
struct FBBBAimAnimationConfig;
struct FBBBAimRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationConfig;
struct FBBBCharacterAnimationState;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterWorldRuntimeData;
class FBBBCharacterInitializer;
class UCharacterMovementComponent;
class USkeletalMeshComponent;

//角色动画系统
class ABBB_EVAC_API FBBBCharacterAnimationSystem final
{
public:

    /**
     * 每帧驱动全部动画处理器 更新动画表现状态
     */
    void Update();
private:
    
    friend class FBBBCharacterInitializer;

    /**
     * 初始化动画系统依赖与配置
     * @param InCharacterMesh	角色骨骼网格组件
     * @param InMovement	角色移动组件
     * @param InAnimationData	动画运行时数据
     * @param InAnimationState	动画状态
     * @param InWorldData	世界运行时数据
     * @param InAimData	瞄准运行时数据
     * @param InEquipmentState	装备状态
     * @param InLocomotionConfig	角色移动配置
     * @param InAimAnimationConfig	瞄准动画配置
     */
    void Initialize(
        USkeletalMeshComponent &InCharacterMesh,
        UCharacterMovementComponent &InMovement,
        FBBBAnimationRuntimeData &InAnimationData,
        FBBBCharacterAnimationState &InAnimationState,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBAimRuntimeData &InAimData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBCharacterLocomotionConfig &InLocomotionConfig,
        const FBBBCharacterAnimationConfig &InAnimationConfig,
        const FBBBAimAnimationConfig &InAimAnimationConfig);

    FBBBAnimationRuntimeData *AnimationData = nullptr;
    FBBBCharacterAnimationState *AnimationState = nullptr;
    const FBBBAimRuntimeData *AimData = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;
    const FBBBCharacterLocomotionConfig *LocomotionConfig = nullptr;
    USkeletalMeshComponent *CharacterMesh = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    UCharacterMovementComponent *Movement = nullptr;
    const FBBBCharacterAnimationConfig *AnimationConfig = nullptr;
    const FBBBAimAnimationConfig *AimAnimationConfig = nullptr;
    FBBBCharacterAnimationProcessor AnimationProcessor;
    FBBBCharacterAimPresentationProcessor AimPresentationProcessor;
    FBBBCharacterLocomotionFactsProcessor LocomotionFactsProcessor;
    FBBBCharacterEquipmentPoseProcessor EquipmentPoseProcessor;
};
