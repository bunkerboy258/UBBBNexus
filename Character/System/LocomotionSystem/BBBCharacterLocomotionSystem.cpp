
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionSystem::Initialize(
    ACharacter &InPawn,
    UCharacterMovementComponent &InMovement,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    Character = &InPawn;
    Movement = &InMovement;
    IntentData = &InIntentData;
    Config = &InConfig;
}

void FBBBCharacterLocomotionSystem::Update()
{
    if (!ensureMsgf(Character && Movement && IntentData && Config, TEXT("[UBBBC]Locomotion system update failed because dependencies are null")))
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

    if (IntentData->WantsJump())
    {
        Character->Jump();
    }

    if (!IntentData->HasMoveInput())
    { return; }
    const float InputScale = FMath::Clamp(DesiredSpeed / NetworkMaxSpeed, 0.0f, 1.0f);
    const FRotator YawRotation(0.0f, Character->GetControlRotation().Yaw, 0.0f);

    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X),
        IntentData->GetMoveInput().Y * InputScale);

    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y),
        IntentData->GetMoveInput().X * InputScale);
}
