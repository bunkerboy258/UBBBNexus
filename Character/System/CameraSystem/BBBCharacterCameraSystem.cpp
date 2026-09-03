
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Definition/BBBCameraRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
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
    const FBBBCharacterEquipmentEvents &InEquipmentEvents,
    const FBBBCharacterCameraConfig &InConfig)
{
    Pawn = &InPawn;
    CameraBoom = &InCameraBoom;
    CameraData = &InCameraData;
    WorldData = &InWorldData;
    InputData = &InInputData;
    IntentData = &InIntentData;
    EquipmentEvents = &InEquipmentEvents;
    Config = &InConfig;
}

void FBBBCharacterCameraSystem::Update()
{
    if (!ensureMsgf(Pawn && CameraBoom && WorldData && InputData && IntentData && EquipmentEvents && CameraData && Config, TEXT("[UBBBC]Camera system update failed because dependencies are null")))
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
    for (const FBBBEquipmentRecoilEvent &Event : EquipmentEvents->GetRecoilEvents())
    {
        if (Event.RecoverySpeed > 0.0f)
        {
            State.RecoilRecoverySpeed = Event.RecoverySpeed;
        }
        if (AController *Controller = Pawn->GetController())
        {
            FRotator Rotation = Controller->GetControlRotation();
            Rotation.Pitch += Event.Impulse.X;
            Rotation.Yaw += Event.Impulse.Y;

            Controller->SetControlRotation(Rotation);
        }
        State.AppliedRecoilOffset += Event.Impulse;
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
