
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionSystem::Initialize(
    ACharacter &InPawn,
    UCharacterMovementComponent &InMovement,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    Character = &InPawn;
    Movement = &InMovement;
    IntentData = &InIntentData;
    EquipmentState = &InEquipmentState;
    Config = &InConfig;
}

void FBBBCharacterLocomotionSystem::Update()
{
    if (!ensureMsgf(Character && Movement && IntentData && EquipmentState && Config, TEXT("[UBBBC]Locomotion system update failed because dependencies are null")))
    {
        return;
    }

    const bool bHasMainHandEquipment = EquipmentState->GetActiveMainHandInstance() != nullptr;
    const FBBBCharacterLocomotionProfileConfig *Profile = &Config->Unarmed;

    if (bHasMainHandEquipment)
    {
        Profile = &Config->MainHandEquipped;
    }

    float DesiredSpeed = Profile->WalkSpeed;
    float DesiredAcceleration = Profile->WalkAcceleration;

    if (IntentData->WantsSprint())
    {
        DesiredSpeed = Profile->RunSpeed;
        DesiredAcceleration = Profile->RunAcceleration;
    }

    Movement->MaxWalkSpeed = FMath::Max(DesiredSpeed, 1.0f);
    Movement->MaxAcceleration = FMath::Max(DesiredAcceleration, 0.0f);
    Movement->bOrientRotationToMovement = !IntentData->WantsAim();

    if (IntentData->WantsJump())
    {
        Character->Jump();
    }

    if (!IntentData->HasMoveInput())
    {
        return;
    }

    const FRotator YawRotation(0.0f, Character->GetControlRotation().Yaw, 0.0f);

    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X),
        IntentData->GetMoveInput().Y);

    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y),
        IntentData->GetMoveInput().X);
}
