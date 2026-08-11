#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
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
    const FBBBAimAnimationConfig &InAimAnimationConfig)
{
    CharacterMesh = &InCharacterMesh;
    Movement = &InMovement;
    AnimationData = &InAnimationData;
    AnimationState = &InAnimationState;
    WorldData = &InWorldData;
    AimData = &InAimData;
    EquipmentState = &InEquipmentState;
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
            && AimAnimationConfig,
        TEXT("[UBBBC]Animation system update failed because dependencies are null")))
    { return; }

    AimPresentationProcessor.Update(
        *CharacterMesh,
        WorldData->GetFrameDeltaSeconds(),
        *AimAnimationConfig,
        *AimData,
        *AnimationData,
        *AnimationState);

    LocomotionFactsProcessor.Update(
        *Movement,
        *EquipmentState,
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
