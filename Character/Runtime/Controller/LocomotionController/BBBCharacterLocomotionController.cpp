#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/BBBCharacterLocomotionController.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionController::Initialize(
    ACharacter &InCharacter,
    UCharacterMovementComponent &InMovement,
    FBBBCharacterLocomotionRuntimeData &InRuntimeData,
    const FBBBCharacterControlState &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    Character = &InCharacter;
    Movement = &InMovement;
    RuntimeData = &InRuntimeData;
    ControlData = &InIntentData;
    Config = &InConfig;
    StrafeSpeedMapCurve = InConfig.StrafeSpeedMapCurve.LoadSynchronous();
    ensureMsgf(StrafeSpeedMapCurve, TEXT("[UBBBC]Locomotion initialization failed to load official strafe speed map curve"));
}

void FBBBCharacterLocomotionController::Update()
{
    if (!ensureMsgf(Character && Movement && RuntimeData && ControlData && Config && StrafeSpeedMapCurve,
        TEXT("[UBBBC]Locomotion controller dependencies are null")))
    {
        return;
    }

    LocomotionProcessor.Update(*Character, *Movement, *RuntimeData, *ControlData, *Config,
        *StrafeSpeedMapCurve);
}
