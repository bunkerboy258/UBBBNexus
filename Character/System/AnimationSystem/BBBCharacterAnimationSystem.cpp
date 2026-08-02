
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimTypes.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionTypes.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationState.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"
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
    {
        return;
    }

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
