#include "BBBWork/UBBBNexus/PlayerCamera/Processors/BBBPlayerCameraImpulseProcessor.h"

#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

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

    UAnimInstance *Source = nullptr;
    int32 FireSequence = 0;
    FBBBPlayerCameraRecoilSettings Settings = CameraSystem.DefaultRecoilSettings;
    UBBBAnimInstance *CharacterAnimation = Cast<UBBBAnimInstance>(CameraSystem.Character->GetMesh()->GetAnimInstance());
    const bool bHasSource = CameraSystem.ReadRecoilSource(CharacterAnimation, Source, FireSequence, Settings);
    if (bHasSource && Source)
    {
        if (CameraSystem.RecoilSource.Get() != Source)
        {
            CameraSystem.RecoilSource = Source;
            CameraSystem.LastFireSequence = FireSequence;
        }

        CameraSystem.ActiveRecoilSettings = Settings;
        if (FireSequence != CameraSystem.LastFireSequence)
        {
            CameraSystem.LastFireSequence = FireSequence;
            FBBBPlayerCameraInput Shot;
            Shot.Impulse = Settings.ImpulseDegrees + FVector(
                FMath::FRandRange(-Settings.RandomDegrees.X, Settings.RandomDegrees.X),
                FMath::FRandRange(-Settings.RandomDegrees.Y, Settings.RandomDegrees.Y),
                FMath::FRandRange(-Settings.RandomDegrees.Z, Settings.RandomDegrees.Z));
            CameraSystem.Submit(Shot);
        }
    }

    if (!bHasSource || !Source)
    {
        CameraSystem.RecoilSource.Reset();
        CameraSystem.LastFireSequence = 0;
    }

    const FVector Previous = CameraSystem.RecoilOffset;
    const FVector Limit = CameraSystem.ActiveRecoilSettings.LimitDegrees;
    const float RecoverySpeed = CameraSystem.ActiveRecoilSettings.RecoverySpeed;
    if (!ensureMsgf(
        FMath::IsFinite(RecoverySpeed) && RecoverySpeed > 0.0f
            && !Limit.ContainsNaN() && Limit.X > 0.0f && Limit.Y > 0.0f && Limit.Z > 0.0f,
        TEXT("相机冲击恢复速度或角度限制无效")))
    {
        CameraSystem.Pending.Reset();
        return;
    }

    CameraSystem.RecoilOffset *= FMath::Exp(-RecoverySpeed * FMath::Max(DeltaSeconds, 0.0f));
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
