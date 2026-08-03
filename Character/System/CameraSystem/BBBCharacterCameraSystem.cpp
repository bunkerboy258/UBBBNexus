
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Definition/BBBCameraRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"

void FBBBCharacterCameraSystem::Initialize(
    APawn &InPawn,
    USpringArmComponent &InCameraBoom,
    FBBBCameraRuntimeData &InCameraData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBInputRuntimeData &InInputData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterCameraConfig &InConfig)
{
    Pawn = &InPawn;
    CameraBoom = &InCameraBoom;
    CameraData = &InCameraData;
    WorldData = &InWorldData;
    InputData = &InInputData;
    IntentData = &InIntentData;
    Config = &InConfig;
}

void FBBBCharacterCameraSystem::Update()
{
    if (!ensureMsgf(Pawn && CameraBoom && WorldData && InputData && IntentData && CameraData && Config, TEXT("[UBBBC]Camera system update failed because dependencies are null")))
    { return; }
    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();
    const FBBBProcessedInputFrame &ProcessedInput = InputData->GetProcessedInput();
    const FVector2D Look = ProcessedInput.LookDelta;

    Pawn->AddControllerYawInput(Look.X * Config->BaseTurnRate);
    Pawn->AddControllerPitchInput(Look.Y * Config->BaseTurnRate);
    float TargetLength = Config->CameraBoomLength;
    if (IntentData->WantsAim())
    {
        TargetLength = Config->AimBoomLength;
    }

    CameraBoom->TargetArmLength = FMath::FInterpTo(
        CameraBoom->TargetArmLength,
        TargetLength,
        DeltaSeconds,
        Config->AimBoomInterpSpeed);
    FBBBCameraState State = CameraData->GetState();
    const FBBBCameraCommands Commands = CameraData->ConsumeRecoilRequest();
    if (!Commands.PendingRecoilImpulse.IsNearlyZero())
    {
        if (Commands.PendingRecoilRecoverySpeed > 0.0f)
        {
            State.RecoilRecoverySpeed = Commands.PendingRecoilRecoverySpeed;
        }
        if (AController *Controller = Pawn->GetController())
        {
            FRotator Rotation = Controller->GetControlRotation();
            Rotation.Pitch += Commands.PendingRecoilImpulse.X;
            Rotation.Yaw += Commands.PendingRecoilImpulse.Y;

            Controller->SetControlRotation(Rotation);
        }
        State.AppliedRecoilOffset += Commands.PendingRecoilImpulse;
    }
    const FVector2D NewOffset(
        FMath::FInterpTo(State.AppliedRecoilOffset.X, 0.0f, DeltaSeconds, State.RecoilRecoverySpeed),
        FMath::FInterpTo(State.AppliedRecoilOffset.Y, 0.0f, DeltaSeconds, State.RecoilRecoverySpeed));
    const FVector2D OffsetDelta = NewOffset - State.AppliedRecoilOffset;
    State.AppliedRecoilOffset = NewOffset;
    if (AController *Controller = Pawn->GetController())
    {
        FRotator Rotation = Controller->GetControlRotation();
        Rotation.Pitch += OffsetDelta.X;
        Rotation.Yaw += OffsetDelta.Y;

        Controller->SetControlRotation(Rotation);
    }

    CameraData->CommitState(State);
}
