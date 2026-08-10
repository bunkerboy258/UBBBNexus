#include "BBBWork/UBBBNexus/Character/System/AimSystem/Processors/BBBCharacterAimFacingProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterAimFacingProcessor::Update(
    UCharacterMovementComponent &Movement,
    const FBBBAimRuntimeState &State) const
{
    if (!State.bIsAiming)
    {
        Movement.bOrientRotationToMovement = true;
        Movement.bUseControllerDesiredRotation = false;
        return;
    }

    Movement.bOrientRotationToMovement = false;
    Movement.bUseControllerDesiredRotation = true;
}
