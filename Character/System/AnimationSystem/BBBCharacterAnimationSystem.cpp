#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBCharacterAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
    ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InRuntimeData,
    USkeletalMeshComponent &InCharacterMesh,
    FBBBAnimationRuntimeData &InAnimationData,
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterAnimationConfig &InAnimationConfig)
{
    Character = &InCharacter;
    RuntimeData = &InRuntimeData;
    CharacterMesh = &InCharacterMesh;
    AnimationData = &InAnimationData;
    EquipmentEvents = &InEquipmentEvents;
    EquipmentState = &InEquipmentState;
    WorldData = &InWorldData;
    IntentData = &InIntentData;
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
            && IntentData
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

    if (IntentData->WantsDash())
    {
        const FVector2D MoveInput = IntentData->GetMoveInput();
        UAnimMontage *DashMontage = AnimationConfig->DashForwardMontage;

        if (FMath::Abs(MoveInput.X) > FMath::Abs(MoveInput.Y))
        {
            DashMontage = MoveInput.X < 0.0f
                ? AnimationConfig->DashLeftMontage
                : AnimationConfig->DashRightMontage;
        }

        if (MoveInput.Y < -0.5f)
        {
            DashMontage = AnimationConfig->DashBackwardMontage;
        }

        AnimInstance->PlayMovementActionMontage(DashMontage);
    }

    if (IntentData->WantsSlide())
    {
        AnimInstance->PlayMovementActionMontage(AnimationConfig->SlideMontage);
    }
    FactProcessor.Update(
        *Character,
        *RuntimeData,
        AnimationData->Facts,
        WorldData->GetFrameDeltaSeconds());
    AnimInstance->PublishAnimationFacts(AnimationData->Facts);
}
