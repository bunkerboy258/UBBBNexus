#include "BBBWork/UBBBNexus/Character/System/AimSystem/Processors/BBBCharacterAimTurnInPlaceProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimTurnInPlaceProcessor::Update(
    APawn &Pawn,
    UCharacterMovementComponent &Movement,
    float DeltaSeconds,
    const FBBBAimConfig &Config,
    FBBBAimRuntimeState &State) const
{
    Pawn.bUseControllerRotationYaw = false;
    Movement.bOrientRotationToMovement = false;
    Movement.bUseControllerDesiredRotation = false;

    AController *Controller = Pawn.GetController();
    if (!ensureMsgf(Controller, TEXT("[UBBBC]Turn in place update failed because controller is null")))
    {
        State.bIsTurningInPlace = false;
        return;
    }

    const float ActorYaw = Pawn.GetActorRotation().Yaw;
    const float ControlYaw = Controller->GetControlRotation().Yaw;
    const float AimOffsetYawAbs = FMath::Abs(FMath::FindDeltaAngleDegrees(ActorYaw, ControlYaw));

    if (!State.bIsTurningInPlace && AimOffsetYawAbs <= Config.TurnInPlaceStartAngle)
    {
        State.bIsTurningInPlace = false;
        return;
    }

    if (State.bIsTurningInPlace && AimOffsetYawAbs <= Config.TurnInPlaceStopAngle)
    {
        State.bIsTurningInPlace = false;
        return;
    }

    const FRotator CurrentRotation = Pawn.GetActorRotation();
    const FRotator TargetRotation(0.0f, ControlYaw, 0.0f);
    const FRotator UpdatedRotation = FMath::RInterpTo(
        CurrentRotation,
        TargetRotation,
        DeltaSeconds,
        FMath::Max(Config.TurnInPlaceInterpSpeed, 0.0f));

    Pawn.SetActorRotation(UpdatedRotation);
    State.bIsTurningInPlace = true;
}
