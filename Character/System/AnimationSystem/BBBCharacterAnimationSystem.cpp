#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
    ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InRuntimeData,
    USkeletalMeshComponent &InCharacterMesh,
    FBBBAnimationRuntimeData &InAnimationData,
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBCharacterAnimationConfig &InAnimationConfig)
{
    Character = &InCharacter;
    RuntimeData = &InRuntimeData;
    CharacterMesh = &InCharacterMesh;
    AnimationData = &InAnimationData;
    EquipmentEvents = &InEquipmentEvents;
    WorldData = &InWorldData;
    AnimationConfig = &InAnimationConfig;
    AnimationData->RequestedAnimationLayerClass = AnimationConfig->DefaultAnimationLayerClass;
}

//------------------------------------------------------------------------------

void FBBBCharacterAnimationSystem::Update()
{
    if (!ensureMsgf(
        AnimationData
            && Character
            && RuntimeData
            && EquipmentEvents
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

    RefreshLinkedAnimationLayer();
    ActionProcessor.Update(
        *AnimInstance,
        *EquipmentEvents,
        *AnimationConfig);
    FactProcessor.Update(
        *Character,
        *RuntimeData,
        AnimationData->Facts,
        WorldData->GetFrameDeltaSeconds());
    AnimInstance->PublishAnimationFacts(AnimationData->Facts);
}

//------------------------------------------------------------------------------

void FBBBCharacterAnimationSystem::RefreshLinkedAnimationLayer()
{
    TSubclassOf<UAnimInstance> DesiredLayerClass = AnimationData->RequestedAnimationLayerClass;

    if (!ensureMsgf(
        DesiredLayerClass,
        TEXT("[UBBBC]No animation layer is configured for the character or active equipment")))
    {
        return;
    }

    if (AnimationData->LinkedAnimationLayerClass == DesiredLayerClass)
    {
        return;
    }

    CharacterMesh->LinkAnimClassLayers(DesiredLayerClass);
    AnimationData->LinkedAnimationLayerClass = DesiredLayerClass;
}

//------------------------------------------------------------------------------

void FBBBCharacterAnimationSystem::SetLinkedAnimationLayerClass(
    TSubclassOf<UAnimInstance> AnimationLayerClass)
{
    if (!ensureMsgf(
        AnimationData && AnimationConfig,
        TEXT("[UBBBC]Animation layer request failed because animation system is not initialized")))
    {
        return;
    }

    AnimationData->RequestedAnimationLayerClass = AnimationLayerClass
        ? AnimationLayerClass
        : AnimationConfig->DefaultAnimationLayerClass;

    RefreshLinkedAnimationLayer();
}
