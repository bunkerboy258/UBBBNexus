#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Animation/BBBMinigunAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"

void UBBBMinigunAnimInstance::PublishMinigunSnapshot(
    const int32 InLoadedAmmo,
    const int32 InAmmoCapacity,
    const bool bInReloading,
    const bool bInSpinning,
    const float InTimeSinceLastFireSeconds,
    const int32 InFireSequence,
    const float InSnapshotTimeSeconds,
    const UBBBMinigunDefinition &Definition)
{
    // 游戏线程统一发布 动画图不再读取装备可变黑板
    check(IsInGameThread());
    LoadedAmmo = InLoadedAmmo;
    AmmoCapacity = InAmmoCapacity;
    this->bIsReloading = bInReloading;
    bIsSpinning = bInSpinning;
    SpinUpSeconds = Definition.SpinUpSeconds;
    SpinDownSeconds = Definition.SpinDownSeconds;
    BarrelRotationSpeedDegrees = Definition.BarrelRotationSpeedDegrees;
    TimeSinceLastFireSeconds = InTimeSinceLastFireSeconds;
    FireSequence = InFireSequence;
    SnapshotTimeSeconds = InSnapshotTimeSeconds;
    RecoilAnimation = Definition.CharacterRecoilAnimation;
    HipFireAimFollowSpeed = Definition.HipFireSettings.AimFollowSpeed;
    AimFireAimFollowSpeed = Definition.AimFireSettings.AimFollowSpeed;
    HipFireBackwardRecoilAlpha = Definition.HipFireSettings.BackwardRecoilAlpha;
    AimFireBackwardRecoilAlpha = Definition.AimFireSettings.BackwardRecoilAlpha;
    HipFireSwayAmplitudeDegrees = Definition.HipFireSettings.SwayAmplitudeDegrees;
    AimFireSwayAmplitudeDegrees = Definition.AimFireSettings.SwayAmplitudeDegrees;
    HipFireSwayFrequency = Definition.HipFireSettings.SwayFrequency;
    AimFireSwayFrequency = Definition.AimFireSettings.SwayFrequency;
    AirborneAimFollowScale = Definition.AirborneModifiers.AimFollowScale;
    AirborneBackwardRecoilScale = Definition.AirborneModifiers.BackwardRecoilScale;
    AirborneSwayAmplitudeScale = Definition.AirborneModifiers.SwayAmplitudeScale;
    AirborneSwayFrequencyScale = Definition.AirborneModifiers.SwayFrequencyScale;
    HipFireCameraSettings = Definition.HipFireCameraSettings;
    AimFireCameraSettings = Definition.AimFireCameraSettings;
    AirborneCameraImpulseScale = Definition.AirborneCameraImpulseScale;
    AirborneCameraRecoveryScale = Definition.AirborneCameraRecoveryScale;
}
