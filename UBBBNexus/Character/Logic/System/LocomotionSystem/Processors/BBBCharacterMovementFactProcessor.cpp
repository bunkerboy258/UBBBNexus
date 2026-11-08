#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterMovementFactProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterMovementFactContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterMovementFactProcessor::Update(FBBBCharacterMovementFactContext &Context) const
{
    auto &State = Context.RuntimeData.Locomotion.LocomotionState;
    const auto &Control = Context.RuntimeData.Parse.ReadControlState();
    State.Acceleration = Context.Movement.GetCurrentAcceleration();
    State.MovementInput = Control.MoveWorld;
    State.FacingWorld = Control.FacingWorld;
    if (Context.bRestoreFacts)
    {
        State.Acceleration = State.RestoredAcceleration;
        State.MovementInput = State.RestoredMovementInput;
        State.FacingWorld = Context.Character.GetActorRotation();
    }
}
