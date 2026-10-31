#include "BBBWork/UBBBNexus/Player/Camera/Processors/BBBPlayerCameraImpulseProcessor.h"

#include "BBBWork/UBBBNexus/Player/Camera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

void FBBBPlayerCameraImpulseProcessor::Update(ABBBPlayerCameraSystem &CameraSystem, const float DeltaSeconds)
{
    if (!CameraSystem.Character.IsValid() || !CameraSystem.Controller.IsValid())
    {
        return;
    }

    const auto &CameraInput = CameraSystem.Character->RuntimeData.Parse.ReadCameraState();
    if (CameraInput.Revision != CameraSystem.LastCameraRevision)
    {
        CameraSystem.LastCameraRevision = CameraInput.Revision;
        CameraSystem.Submit(CameraInput.LatestInput);
    }

    UAnimInstance *Source = nullptr;
    int32 FireSequence = 0;
    float RetainedPitch = 0.0f;
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
        const int32 NewShots = FMath::Max(0, FireSequence - CameraSystem.LastFireSequence);
        CameraSystem.LastFireSequence = FireSequence;
        for (int32 ShotIndex = 0; ShotIndex < NewShots; ++ShotIndex)
        {
            FBBBPlayerCameraInput Shot;
            Shot.Impulse = Settings.ImpulseDegrees + FVector(
                FMath::FRandRange(-Settings.RandomDegrees.X, Settings.RandomDegrees.X),
                FMath::FRandRange(-Settings.RandomDegrees.Y, Settings.RandomDegrees.Y),
                FMath::FRandRange(-Settings.RandomDegrees.Z, Settings.RandomDegrees.Z));
            CameraSystem.Submit(Shot);
            if (!Shot.Impulse.ContainsNaN())
            {
                RetainedPitch += Shot.Impulse.X * Settings.PitchRetention;
            }
        }
    }

    if (!bHasSource || !Source)
    {
        CameraSystem.RecoilSource.Reset();
        CameraSystem.LastFireSequence = 0;
        if (CameraSystem.Pending.IsSet() || CameraSystem.RecoilOffset.IsNearlyZero(0.001))
        {
            CameraSystem.ActiveRecoilSettings = CameraSystem.DefaultRecoilSettings;
        }
    }

    const FVector Previous = CameraSystem.RecoilOffset;
    const FVector Limit = CameraSystem.ActiveRecoilSettings.LimitDegrees;
    const float RecoverySpeed = CameraSystem.ActiveRecoilSettings.RecoverySpeed;
    const float PitchRetention = CameraSystem.ActiveRecoilSettings.PitchRetention;
    if (!ensureMsgf(
        FMath::IsFinite(RecoverySpeed) && RecoverySpeed > 0.0f
            && FMath::IsFinite(PitchRetention) && PitchRetention >= 0.0f && PitchRetention <= 1.0f
            && !Limit.ContainsNaN() && Limit.X > 0.0f && Limit.Y > 0.0f && Limit.Z > 0.0f,
        TEXT("相机冲击恢复速度 保留比例或角度限制无效")))
    {
        CameraSystem.Pending.Reset();
        return;
    }

    CameraSystem.RecoilOffset *= FMath::Exp(-RecoverySpeed * FMath::Max(DeltaSeconds, 0.0f));
    if (CameraSystem.Pending.IsSet())
    {
        CameraSystem.RecoilOffset += CameraSystem.Pending->Impulse;
        CameraSystem.RecoilOffset.X -= RetainedPitch;
        CameraSystem.Pending.Reset();
    }

    CameraSystem.RecoilOffset.X = FMath::Clamp(CameraSystem.RecoilOffset.X, -Limit.X, Limit.X);
    CameraSystem.RecoilOffset.Y = FMath::Clamp(CameraSystem.RecoilOffset.Y, -Limit.Y, Limit.Y);
    CameraSystem.RecoilOffset.Z = FMath::Clamp(CameraSystem.RecoilOffset.Z, -Limit.Z, Limit.Z);
    const FVector Delta = CameraSystem.RecoilOffset - Previous + FVector(RetainedPitch, 0.0f, 0.0f);
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
        bAiming ? CameraSystem.AimArmLength : CameraSystem.HipArmLength,
        DeltaSeconds,
        CameraSystem.AimBoomInterpSpeed);
    CameraSystem.Boom->SocketOffset.Y = FMath::FInterpTo(
        CameraSystem.Boom->SocketOffset.Y,
        bAiming ? CameraSystem.AimShoulderOffset : CameraSystem.HipShoulderOffset,
        DeltaSeconds,
        CameraSystem.AimBoomInterpSpeed);
    CameraSystem.Camera->SetFieldOfView(FMath::FInterpTo(
        CameraSystem.Camera->FieldOfView,
        bAiming ? CameraSystem.AimFieldOfView : CameraSystem.HipFieldOfView,
        DeltaSeconds,
        CameraSystem.AimBoomInterpSpeed));
    const float CapsuleHalfHeight = CameraSystem.Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    if (CapsuleHalfHeight != CameraSystem.CapsuleHeightBlendTarget)
    {
        CameraSystem.CapsuleHeightBlendStart = CameraSystem.SmoothedCapsuleHalfHeight;
        CameraSystem.CapsuleHeightBlendTarget = CapsuleHalfHeight;
        CameraSystem.CapsuleHeightBlendElapsed = 0.0f;
    }

    const float BlendDuration = FMath::Max(CameraSystem.StanceHeightBlendDuration, 0.01f);
    CameraSystem.CapsuleHeightBlendElapsed = FMath::Min(
        CameraSystem.CapsuleHeightBlendElapsed + FMath::Max(DeltaSeconds, 0.0f), BlendDuration);
    const float HeightAlpha = FMath::SmoothStep(0.0f, 1.0f,
        CameraSystem.CapsuleHeightBlendElapsed / BlendDuration);
    CameraSystem.SmoothedCapsuleHalfHeight = FMath::Lerp(
        CameraSystem.CapsuleHeightBlendStart, CameraSystem.CapsuleHeightBlendTarget, HeightAlpha);

    // 抵消胶囊中心的瞬时高度变化 仅保留姿态高度的平滑过渡
    FVector CameraLocation = CameraSystem.Character->GetActorLocation();
    CameraLocation.Z += CameraSystem.SmoothedCapsuleHalfHeight - CapsuleHalfHeight;
    CameraSystem.SetActorLocationAndRotation(CameraLocation, Rotation);
}
