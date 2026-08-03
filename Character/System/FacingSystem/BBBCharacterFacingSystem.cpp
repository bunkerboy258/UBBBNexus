
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/BBBCharacterFacingSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBFacingTypes.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeTypes.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterFacingSystem::Initialize(
    APawn &InPawn,
    UCharacterMovementComponent &InMovement,
    FBBBFacingRuntimeData &InFacingData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBAimRuntimeData &InAimData,
    const FBBBCharacterFacingConfig &InConfig)
{
    Pawn = &InPawn;
    Movement = &InMovement;
    FacingData = &InFacingData;
    WorldData = &InWorldData;
    IntentData = &InIntentData;
    AimData = &InAimData;
    Config = &InConfig;
}

void FBBBCharacterFacingSystem::Update()
{
    if (!ensureMsgf(Pawn && Movement && WorldData && FacingData && IntentData && AimData && Config, TEXT("[UBBBC]Facing system update failed because dependencies are null")))
    { return; }
    bool bIsBodyTurning = FacingData->IsBodyTurning();
    float CurrentAimYawAbs = FacingData->GetCurrentAimYawAbs();
    bool bLastFaceAimDirection = FacingData->GetLastFaceAimDirection();
    bool bHasAppliedRotationMode = FacingData->HasAppliedRotationMode();
    const bool bFaceAimDirection = IntentData->WantsAim();
    if (!bHasAppliedRotationMode || bFaceAimDirection != bLastFaceAimDirection)
    {
        bHasAppliedRotationMode = true;
        bLastFaceAimDirection = bFaceAimDirection;
        bIsBodyTurning = false;
        Movement->bOrientRotationToMovement = !bFaceAimDirection;
        Movement->bUseControllerDesiredRotation = false;
    }
    if (!bFaceAimDirection || !Pawn->GetController())
    {
        bIsBodyTurning = false;
        FacingData->CommitState(
            bIsBodyTurning,
            CurrentAimYawAbs,
            bLastFaceAimDirection,
            bHasAppliedRotationMode);
        return;
    }
    const FBBBAimRuntimeState &AimState = AimData->GetState();
    CurrentAimYawAbs = FMath::Abs(AimState.AimYaw);
    if (!bIsBodyTurning && CurrentAimYawAbs <= Config->MaxAimYawBeforeBodyTurn)
    {
        FacingData->CommitState(
            bIsBodyTurning,
            CurrentAimYawAbs,
            bLastFaceAimDirection,
            bHasAppliedRotationMode);
        return;
    }
    if (bIsBodyTurning && CurrentAimYawAbs <= Config->AimYawBodyTurnStopThreshold)
    {
        bIsBodyTurning = false;
        FacingData->CommitState(
            bIsBodyTurning,
            CurrentAimYawAbs,
            bLastFaceAimDirection,
            bHasAppliedRotationMode);
        return;
    }
    bIsBodyTurning = true;
    const FVector ToTarget = FVector(AimState.AimTargetWorld) - AimData->GetAimOriginWorld();
    if (ToTarget.IsNearlyZero())
    {
        FacingData->CommitState(
            bIsBodyTurning,
            CurrentAimYawAbs,
            bLastFaceAimDirection,
            bHasAppliedRotationMode);
        return;
    }
    const FRotator TargetRotation(0.0f, ToTarget.Rotation().Yaw, 0.0f);

    Pawn->SetActorRotation(
        FMath::RInterpTo(
            Pawn->GetActorRotation(),
            TargetRotation,
            WorldData->GetFrameDeltaSeconds(),
            Config->ArmedBodyTurnInterpSpeed));

    FacingData->CommitState(
        bIsBodyTurning,
        CurrentAimYawAbs,
        bLastFaceAimDirection,
        bHasAppliedRotationMode);
}
