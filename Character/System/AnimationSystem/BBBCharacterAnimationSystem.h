#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAimPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterEquipmentPoseProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionStateProcessor.h"
struct FBBBAimAnimationConfig;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationState;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterLocomotionConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;
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
     * @param InFacingData	朝向运行时数据
     * @param InIntentData	意图运行时数据
     * @param InEquipmentState	装备状态
     * @param InAimConfig	瞄准配置
     * @param InAimAnimationConfig	瞄准动画配置
     * @param InLocomotionConfig	Locomotion配置
     * @param InAimSourceBoneName	瞄准来源骨骼名
     */
    void Initialize(
        USkeletalMeshComponent &InCharacterMesh,
        UCharacterMovementComponent &InMovement,
        FBBBAnimationRuntimeData &InAnimationData,
        FBBBCharacterAnimationState &InAnimationState,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBAimRuntimeData &InAimData,
        const FBBBFacingRuntimeData &InFacingData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBAimConfig &InAimConfig,
        const FBBBAimAnimationConfig &InAimAnimationConfig,
        const FBBBCharacterLocomotionConfig &InLocomotionConfig,
        FName InAimSourceBoneName);

    FBBBAnimationRuntimeData *AnimationData = nullptr;
    FBBBCharacterAnimationState *AnimationState = nullptr;
    const FBBBAimRuntimeData *AimData = nullptr;
    const FBBBFacingRuntimeData *FacingData = nullptr;
    const FBBBIntentRuntimeData *IntentData = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;
    USkeletalMeshComponent *CharacterMesh = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    UCharacterMovementComponent *Movement = nullptr;
    const FBBBAimConfig *AimConfig = nullptr;
    const FBBBAimAnimationConfig *AimAnimationConfig = nullptr;
    const FBBBCharacterLocomotionConfig *LocomotionConfig = nullptr;
    FName AimSourceBoneName = NAME_None;
    FBBBCharacterAnimationProcessor AnimationProcessor;
    FBBBCharacterAimPresentationProcessor AimPresentationProcessor;
    FBBBCharacterLocomotionStateProcessor LocomotionStateProcessor;
    FBBBCharacterEquipmentPoseProcessor EquipmentPoseProcessor;
};
