#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
    ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InRuntimeData,
    USkeletalMeshComponent &InCharacterMesh,
    FBBBAnimationRuntimeData &InAnimationData,
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBCharacterAnimationConfig &InAnimationConfig)
{
    Character = &InCharacter;
    RuntimeData = &InRuntimeData;
    CharacterMesh = &InCharacterMesh;
    AnimationData = &InAnimationData;
    EquipmentEvents = &InEquipmentEvents;
    EquipmentState = &InEquipmentState;
    WorldData = &InWorldData;
    AnimationConfig = &InAnimationConfig;
}

//------------------------------------------------------------------------------

void FBBBCharacterAnimationSystem::Update()
{
    if (!ensureMsgf(
        AnimationData
            && Character
            && RuntimeData
            && EquipmentEvents
            && EquipmentState
            && CharacterMesh
            && WorldData
            && AnimationConfig,
        TEXT("[UBBBC]Animation system update failed because dependencies are null")))
    {
        return;
    }

    UBBBAnimInstance *AnimInstance = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    if (!ensureMsgf(
        AnimInstance,
        TEXT("[UBBBC]Character mesh must use UBBBAnimInstance or a derived animation blueprint")))
    {
        return;
    }

    LayerProcessor.Update(
        *EquipmentState,
        *AnimationConfig,
        *AnimationData,
        *CharacterMesh);
    ActionProcessor.Update(
        *AnimInstance,
        *EquipmentEvents);
    FactProcessor.Update(
        *Character,
        *RuntimeData,
        AnimationData->Facts,
        WorldData->GetFrameDeltaSeconds());
    AnimInstance->PublishAnimationFacts(AnimationData->Facts);
}
