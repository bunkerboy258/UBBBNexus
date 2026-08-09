#include "BBBWork/UBBBNexus/Character/System/AimSystem/Processors/BBBCharacterAimTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimTargetProcessor::Update(
    APawn &Pawn,
    float AimTargetDistance,
    FBBBAimRuntimeState &State) const
{
    AController *Controller = Pawn.GetController();
    if (!ensureMsgf(Controller, TEXT("[UBBBC]Aim target update failed because controller is null")))
    {
        return;
    }

    FVector ViewLocation = FVector::ZeroVector;
    FRotator ViewRotation = FRotator::ZeroRotator;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FVector ViewDirection = ViewRotation.Vector().GetSafeNormal();
    if (!ensureMsgf(!ViewDirection.IsNearlyZero(), TEXT("[UBBBC]Aim target update failed because view direction is invalid")))
    {
        return;
    }

    State.AimTargetWorld = ViewLocation + ViewDirection * FMath::Max(AimTargetDistance, 0.0f);
}
