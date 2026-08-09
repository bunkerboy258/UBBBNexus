#include "BBBWork/UBBBNexus/Character/System/AimSystem/Processors/BBBCharacterAimFacingProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimFacingProcessor::Update(
    APawn &Pawn,
    UCharacterMovementComponent &Movement,
    float DeltaSeconds,
    float FacingInterpSpeed,
    const FBBBAimRuntimeState &State) const
{
    Pawn.bUseControllerRotationYaw = false;

    if (!State.bIsAiming)
    {
        Movement.bOrientRotationToMovement = true;
        Movement.bUseControllerDesiredRotation = false;
        return;
    }

    Movement.bOrientRotationToMovement = false;
    Movement.bUseControllerDesiredRotation = false;

    AController *Controller = Pawn.GetController();
    if (!ensureMsgf(Controller, TEXT("[UBBBC]Aim facing failed because controller is null")))
    {
        return;
    }

    const FRotator CurrentRotation = Pawn.GetActorRotation();
    const FRotator TargetRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
    const FRotator UpdatedRotation = FMath::RInterpTo(
        CurrentRotation,
        TargetRotation,
        DeltaSeconds,
        FMath::Max(FacingInterpSpeed, 0.0f));

    Pawn.SetActorRotation(UpdatedRotation);
}
