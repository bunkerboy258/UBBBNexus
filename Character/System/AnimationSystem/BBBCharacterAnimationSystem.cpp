
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Context/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Context/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Context/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Context/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Context/BBBFacingRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Context/BBBCharacterWorldRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
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
    FName InAimSourceBoneName)
{
    CharacterMesh = &InCharacterMesh;
    Movement = &InMovement;
    AnimationData = &InAnimationData;
    AnimationState = &InAnimationState;
    WorldData = &InWorldData;
    AimData = &InAimData;
    FacingData = &InFacingData;
    IntentData = &InIntentData;
    EquipmentState = &InEquipmentState;
    AimConfig = &InAimConfig;
    AimAnimationConfig = &InAimAnimationConfig;
    LocomotionConfig = &InLocomotionConfig;
    AimSourceBoneName = InAimSourceBoneName;
}

void FBBBCharacterAnimationSystem::Update()
{
    if (!ensureMsgf(
        AnimationData
            && AnimationState
            && AimData
            && FacingData
            && IntentData
            && EquipmentState
            && CharacterMesh
            && WorldData
            && Movement
            && AimConfig
            && AimAnimationConfig
            && LocomotionConfig,
        TEXT("[UBBBC]Animation system update failed because dependencies are null")))
    { return; }

    AimPresentationProcessor.Update(
        *CharacterMesh,
        WorldData->GetFrameDeltaSeconds(),
        *AimConfig,
        *AimAnimationConfig,
        *AimData,
        *AnimationData,
        *AnimationState);

    LocomotionPresentationProcessor.Update(
        *Movement,
        *LocomotionConfig,
        *AimData,
        *IntentData,
        *AnimationState);

    AnimationProcessor.Update(
        *CharacterMesh,
        AnimationData->GetCommands(),
        *AnimationState,
        *IntentData,
        *FacingData,
        *AimData);

    EquipmentPoseProcessor.Update(
        *CharacterMesh,
        //保存瞄准来源BoneName供所属对象后续流程使用
        AimSourceBoneName,
        *EquipmentState,
        AnimationData->GetCommands(),
        *AnimationData,
        *AnimationState);
}
