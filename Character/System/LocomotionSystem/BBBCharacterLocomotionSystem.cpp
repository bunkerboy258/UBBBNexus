
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionTypes.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterLocomotionSystem::Initialize(
    APawn &InPawn,
    UCharacterMovementComponent &InMovement,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    Pawn = &InPawn;
    Movement = &InMovement;
    IntentData = &InIntentData;
    Config = &InConfig;
}

void FBBBCharacterLocomotionSystem::Update()
{
    if (!ensureMsgf(Pawn && Movement && IntentData && Config, TEXT("[UBBBC]Locomotion system update failed because dependencies are null")))
    { return; }
    const float NetworkMaxSpeed = FMath::Max(Config->RunSpeed, 1.0f);
    float DesiredSpeed = 0.0f;
    if (IntentData->HasMoveInput() && IntentData->WantsAim())
    {
        DesiredSpeed = Config->StrafeWalkSpeed;
        if (IntentData->WantsSprint())
        {
            DesiredSpeed = Config->StrafeRunSpeed;
        }
    }
    if (IntentData->HasMoveInput() && !IntentData->WantsAim())
    {
        DesiredSpeed = Config->WalkSpeed;
        if (IntentData->WantsSprint())
        {
            DesiredSpeed = Config->RunSpeed;
        }
    }

    Movement->MaxWalkSpeed = NetworkMaxSpeed;
    Movement->MaxAcceleration = Config->RunAcceleration;
    Movement->bOrientRotationToMovement = !IntentData->WantsAim();
    if (!IntentData->HasMoveInput())
    { return; }
    const float InputScale = FMath::Clamp(DesiredSpeed / NetworkMaxSpeed, 0.0f, 1.0f);
    const FRotator YawRotation(0.0f, Pawn->GetControlRotation().Yaw, 0.0f);

    Pawn->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X),
        IntentData->GetMoveInput().Y * InputScale);

    Pawn->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y),
        IntentData->GetMoveInput().X * InputScale);
}
