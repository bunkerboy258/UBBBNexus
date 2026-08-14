#include "BBBWork/UBBBNexus/Character/System/FacingSystem/BBBCharacterFacingSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBCharacterFacingConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBCharacterFacingRuntimeData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterFacingSystem::Initialize(
    ACharacter &InCharacter,
    UCharacterMovementComponent &InMovement,
    const FBBBIntentRuntimeData &InIntentData,
    FBBBCharacterFacingRuntimeData &InFacingData,
    const FBBBCharacterFacingConfig &InConfig)
{
    Character = &InCharacter;
    Movement = &InMovement;
    IntentData = &InIntentData;
    FacingData = &InFacingData;
    Config = &InConfig;
}

//------------------------------------------------------------------------------

void FBBBCharacterFacingSystem::Update()
{
    if (!ensureMsgf(
        Character && Movement && IntentData && FacingData && Config,
        TEXT("[UBBBC]Facing system update failed because dependencies are null")))
    {
        return;
    }

    if (!ensureMsgf(
        Config->IdleTurnStopAngle >= 0.0f
            && Config->IdleTurnStartAngle > Config->IdleTurnStopAngle
            && Config->IdleTurnStartAngle <= 180.0f
            && Config->IdleTurnRotationRate > 0.0f
            && Config->MoveAlignmentRotationRate > 0.0f,
        TEXT("[UBBBC]Facing system update failed because facing config is invalid")))
    {
        return;
    }

    FBBBCharacterFacingState State = FacingData->GetState();

    FacingProcessor.Update(
        *Character,
        *Movement,
        *IntentData,
        *Config,
        State);

    FacingData->CommitState(State);
}
