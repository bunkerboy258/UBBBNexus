
#include "BBBWork/UBBBNexus/Character/Instance/System/CameraSystem/BBBCharacterCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/CameraSystem/Definition/BBBCameraRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
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
    // 相机更新需要角色相机输入意图事件和运行时状态全部有效
    if (!ensureMsgf(Pawn && CameraBoom && WorldData && InputData && IntentData && EquipmentEvents && CameraData && Config, TEXT("[UBBBC]Camera system update failed because dependencies are null")))
    { return; }
    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();
    const FBBBProcessedInputFrame &ProcessedInput = InputData->GetProcessedInput();
    const FVector2D Look = ProcessedInput.LookDelta;

    // 将本帧视角增量应用到角色控制器
    Pawn->AddControllerYawInput(Look.X * Config->BaseTurnRate);
    Pawn->AddControllerPitchInput(Look.Y * Config->BaseTurnRate);
    float TargetLength = Config->CameraBoomLength;
    // 瞄准时使用更近的相机臂目标距离
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
    // 按事件顺序应用本帧装备后坐力
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
    // 将累计后坐力平滑恢复到零
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

    // 发布本帧相机状态供下一帧继续恢复
    CameraData->CommitState(State);
}
