#include "BBBWork/UBBBNexus/PlayerCamera/Processors/BBBPlayerCameraImpulseProcessor.h"

#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

void FBBBPlayerCameraImpulseProcessor::Update(ABBBPlayerCameraSystem &CameraSystem, const float DeltaSeconds)
{
    if (!CameraSystem.Character.IsValid() || !CameraSystem.Controller.IsValid())
    {
        return;
    }

    auto &CameraInput = CameraSystem.Character->RuntimeData.Parse.CameraState.PendingInput;
    if (CameraInput.IsSet())
    {
        CameraSystem.Submit(CameraInput.GetValue());
        CameraInput.Reset();
    }

    const FVector Previous = CameraSystem.RecoilOffset;
    const FVector Limit = CameraSystem.ImpulseLimitDegrees;
    if (!ensureMsgf(
        FMath::IsFinite(CameraSystem.RecoverySpeed) && CameraSystem.RecoverySpeed > 0.0f
            && !Limit.ContainsNaN() && Limit.X > 0.0f && Limit.Y > 0.0f && Limit.Z > 0.0f,
        TEXT("相机冲击恢复速度或角度限制无效")))
    {
        CameraSystem.Pending.Reset();
        return;
    }

    CameraSystem.RecoilOffset *= FMath::Exp(-CameraSystem.RecoverySpeed * FMath::Max(DeltaSeconds, 0.0f));
    if (CameraSystem.Pending.IsSet())
    {
        CameraSystem.RecoilOffset += CameraSystem.Pending->Impulse;
        CameraSystem.Pending.Reset();
    }

    CameraSystem.RecoilOffset.X = FMath::Clamp(CameraSystem.RecoilOffset.X, -Limit.X, Limit.X);
    CameraSystem.RecoilOffset.Y = FMath::Clamp(CameraSystem.RecoilOffset.Y, -Limit.Y, Limit.Y);
    CameraSystem.RecoilOffset.Z = FMath::Clamp(CameraSystem.RecoilOffset.Z, -Limit.Z, Limit.Z);
    const FVector Delta = CameraSystem.RecoilOffset - Previous;
    FRotator Rotation = CameraSystem.Controller->GetControlRotation();
    const float DesiredPitch = FRotator::NormalizeAxis(Rotation.Pitch) + Delta.X;
    Rotation.Pitch = FMath::Clamp(DesiredPitch, -89.0f, 89.0f);
    CameraSystem.RecoilOffset.X += Rotation.Pitch - DesiredPitch;
    Rotation.Yaw += Delta.Y;
    CameraSystem.Controller->SetControlRotation(Rotation);
    Rotation.Roll += CameraSystem.RecoilOffset.Z;

    const bool bAiming = CameraSystem.Character->RuntimeData.Parse.ReadControlState().bAim;
    CameraSystem.Boom->TargetArmLength = FMath::FInterpTo(
        CameraSystem.Boom->TargetArmLength,
        bAiming ? CameraSystem.AimBoomLength : CameraSystem.DefaultBoomLength,
        DeltaSeconds,
        CameraSystem.AimBoomInterpSpeed);
    CameraSystem.SetActorLocationAndRotation(CameraSystem.Character->GetActorLocation(), Rotation);
}
