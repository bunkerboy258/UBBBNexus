#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/BBBCharacterLocomotionController.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/BBBCharacterLocomotionDomainState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionController::Initialize(
    ACharacter &InCharacter,
    UCharacterMovementComponent &InMovement,
    FBBBCharacterLocomotionDomainState &InRuntimeData,
    const FBBBCharacterControlState &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    Character = &InCharacter;
    Movement = &InMovement;
    RuntimeData = &InRuntimeData;
    ControlData = &InIntentData;
    Config = &InConfig;
    StrafeSpeedMapCurve = InConfig.StrafeSpeedMapCurve.LoadSynchronous();
}

void FBBBCharacterLocomotionController::Update()
{
    if (!Character || !Movement || !RuntimeData || !ControlData || !Config || !StrafeSpeedMapCurve)
    {
        return;
    }

    FBBBCharacterLocomotionUpdateContext Context{
        *Character,
        *Movement,
        RuntimeData->LocomotionState,
        *ControlData,
        *Config,
        *StrafeSpeedMapCurve};
    LocomotionProcessor.Update(Context);
}
