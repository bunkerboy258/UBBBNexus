#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    UCharacterMovementComponent &InMovement,
    FBBBAnimationRuntimeData &InAnimationData,
    FBBBCharacterAnimationState &InAnimationState,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBAimRuntimeData &InAimData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterAnimationConfig &InAnimationConfig,
    const FBBBAimAnimationConfig &InAimAnimationConfig)
{
    CharacterMesh = &InCharacterMesh;
    Movement = &InMovement;
    AnimationData = &InAnimationData;
    AnimationState = &InAnimationState;
    WorldData = &InWorldData;
    AimData = &InAimData;
    EquipmentState = &InEquipmentState;
    AnimationConfig = &InAnimationConfig;
    AimAnimationConfig = &InAimAnimationConfig;
}

void FBBBCharacterAnimationSystem::Update()
{
    if (!ensureMsgf(
        AnimationData
            && AnimationState
            && AimData
            && EquipmentState
            && CharacterMesh
            && WorldData
            && Movement
            && AnimationConfig
            && AimAnimationConfig,
        TEXT("[UBBBC]Animation system update failed because dependencies are null")))
    {
        return;
    }

    if (!ensureMsgf(
        AnimationConfig->TurnSignalRateThreshold > 0.0f
            && AnimationConfig->TurnRateSmoothingTime > 0.0f,
        TEXT("[UBBBC]Animation system update failed because turn configuration is invalid")))
    {
        return;
    }

    AimPresentationProcessor.Update(
        *CharacterMesh,
        WorldData->GetFrameDeltaSeconds(),
        *AimAnimationConfig,
        *AimData,
        AnimationData->GetCommands(),
        *AnimationData,
        *AnimationState);

    LocomotionFactsProcessor.Update(
        *Movement,
        *EquipmentState,
        *AnimationConfig,
        WorldData->GetFrameDeltaSeconds(),
        *AnimationData,
        *AnimationState);

    AnimationProcessor.Update(
        *CharacterMesh,
        AnimationData->GetCommands());

    EquipmentPoseProcessor.Update(
        *EquipmentState,
        AnimationData->GetCommands(),
        *AnimationState);
}
