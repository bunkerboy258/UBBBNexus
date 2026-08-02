
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAimPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterEquipmentPoseProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionPresentationProcessor.h"
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

    void Update();
private:
    
    friend class FBBBCharacterInitializer;

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
    FBBBCharacterLocomotionPresentationProcessor LocomotionPresentationProcessor;
    FBBBCharacterEquipmentPoseProcessor EquipmentPoseProcessor;
};
