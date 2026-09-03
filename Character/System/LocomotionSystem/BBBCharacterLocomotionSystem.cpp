#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
FVector ResolveWorldMoveDirection(
    const ACharacter &Character,
    const FBBBIntentRuntimeData &IntentData)
{
    const FRotator YawRotation(0.0f, Character.GetControlRotation().Yaw, 0.0f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    return (Forward * IntentData.GetMoveInput().Y
        + Right * IntentData.GetMoveInput().X).GetSafeNormal2D();
}

bool CanSprint(
    const ACharacter &Character,
    const FBBBIntentRuntimeData &IntentData,
    const FBBBCharacterLocomotionConfig &Config)
{
    if (!IntentData.WantsSprint())
    {
        return false;
    }

    const FVector WorldMoveDirection = ResolveWorldMoveDirection(Character, IntentData);
    if (WorldMoveDirection.IsNearlyZero())
    {
        return false;
    }

    const float DirectionYaw = WorldMoveDirection.Rotation().Yaw;
    const float DirectionDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(
        Character.GetActorRotation().Yaw,
        DirectionYaw));
    return DirectionDelta < Config.SprintDirectionLimit;
}

EBBBCharacterGait ResolveGait(
    const ACharacter &Character,
    const FBBBIntentRuntimeData &IntentData,
    const FBBBCharacterLocomotionConfig &Config)
{
    const bool bFullMovementInput = IntentData.GetMoveInput().Size() >= Config.AnalogRunThreshold;

    if (CanSprint(Character, IntentData, Config) && bFullMovementInput)
    {
        return EBBBCharacterGait::Sprint;
    }

    if (IntentData.WantsWalk())
    {
        return EBBBCharacterGait::Walk;
    }

    if (bFullMovementInput)
    {
        return EBBBCharacterGait::Run;
    }

    return EBBBCharacterGait::Walk;
}

float ResolveDirectionMap(
    const ACharacter &Character,
    const UCharacterMovementComponent &Movement,
    const UCurveFloat &DirectionCurve)
{
    const FVector LocalVelocity = Character.GetActorTransform().InverseTransformVectorNoScale(
        Movement.Velocity);
    const float DirectionAngle = FMath::Abs(FMath::RadiansToDegrees(
        FMath::Atan2(LocalVelocity.Y, LocalVelocity.X)));
    return FMath::Clamp(DirectionCurve.GetFloatValue(DirectionAngle), 0.0f, 2.0f);
}

float ResolveDirectionalSpeed(const FVector &Speeds, const float DirectionMap)
{
    if (DirectionMap < 1.0f)
    {
        return FMath::Lerp(Speeds.X, Speeds.Y, DirectionMap);
    }

    return FMath::Lerp(Speeds.Y, Speeds.Z, DirectionMap - 1.0f);
}

float ResolveMaxSpeed(
    const FBBBCharacterLocomotionConfig &Config,
    const EBBBCharacterGait Gait,
    const float DirectionMap)
{
    switch (Gait)
    {
        case EBBBCharacterGait::Walk:
            return ResolveDirectionalSpeed(Config.WalkSpeeds, DirectionMap);

        case EBBBCharacterGait::Run:
            return ResolveDirectionalSpeed(Config.RunSpeeds, DirectionMap);

        case EBBBCharacterGait::Sprint:
            return ResolveDirectionalSpeed(Config.SprintSpeeds, DirectionMap);

        case EBBBCharacterGait::Crouch:
            return ResolveDirectionalSpeed(Config.CrouchSpeeds, DirectionMap);
    }

    return Config.WalkSpeeds.X;
}

}

void FBBBCharacterLocomotionSystem::Initialize(
    ACharacter &InCharacter,
    UCharacterMovementComponent &InMovement,
    FBBBCharacterLocomotionRuntimeData &InRuntimeData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    Character = &InCharacter;
    Movement = &InMovement;
    RuntimeData = &InRuntimeData;
    IntentData = &InIntentData;
    Config = &InConfig;
    StrafeSpeedMapCurve = InConfig.StrafeSpeedMapCurve.LoadSynchronous();

    ensureMsgf(
        StrafeSpeedMapCurve,
        TEXT("[UBBBC]Locomotion initialization failed to load official strafe speed map curve"));
}

void FBBBCharacterLocomotionSystem::Update()
{
    if (!ensureMsgf(
        Character && Movement && RuntimeData && IntentData && Config && StrafeSpeedMapCurve,
        TEXT("[UBBBC]Locomotion system update failed because dependencies are null")))
    {
        return;
    }

    const bool bWantsCrouch = IntentData->WantsCrouch();
    if (bWantsCrouch)
    {
        Character->Crouch();
    }

    if (!bWantsCrouch)
    {
        Character->UnCrouch();
    }

    EBBBCharacterGait Gait = ResolveGait(*Character, *IntentData, *Config);
    if (bWantsCrouch)
    {
        Gait = EBBBCharacterGait::Crouch;
    }

    RuntimeData->CommitGait(Gait);
    const float DirectionMap = ResolveDirectionMap(
        *Character,
        *Movement,
        *StrafeSpeedMapCurve);

    Movement->MaxWalkSpeed = FMath::Max(
        ResolveMaxSpeed(*Config, Gait, DirectionMap),
        1.0f);
    Movement->MaxWalkSpeedCrouched = FMath::Max(
        ResolveDirectionalSpeed(Config->CrouchSpeeds, DirectionMap),
        1.0f);
    Movement->MaxAcceleration = FMath::Max(Config->MaxAcceleration, 0.0f);
    Movement->BrakingDecelerationWalking = FMath::Max(Config->BrakingDeceleration, 0.0f);
    Movement->GroundFriction = FMath::Max(Config->GroundFriction, 0.0f);
    Movement->BrakingFriction = FMath::Max(Config->BrakingFriction, 0.0f);
    Movement->BrakingFrictionFactor = FMath::Max(Config->BrakingFrictionFactor, 0.0f);
    Movement->bUseSeparateBrakingFriction = false;

    if (!bWantsCrouch && IntentData->WantsJump())
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
