#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Animation/BBBRifleAnimInstance.h"

void UBBBRifleAnimInstance::PublishRifleSnapshot(
    const int32 InLoadedAmmo,
    const int32 InAmmoCapacity,
    const bool bInReloading,
    const float InTimeSinceLastFireSeconds,
    const int32 InFireSequence,
    const float InSnapshotTimeSeconds,
    const UBBBRifleDefinition &Definition)
{
    // 游戏线程统一发布 动画图不再读取装备可变黑板
    check(IsInGameThread());
    LoadedAmmo = InLoadedAmmo;
    AmmoCapacity = InAmmoCapacity;
    this->bIsReloading = bInReloading;
    TimeSinceLastFireSeconds = InTimeSinceLastFireSeconds;
    FireSequence = InFireSequence;
    SnapshotTimeSeconds = InSnapshotTimeSeconds;
    HipFireSettings = Definition.HipFireSettings;
    AimFireSettings = Definition.AimFireSettings;
    AirborneModifiers = Definition.AirborneModifiers;
    HipFireCameraSettings = Definition.HipFireCameraSettings;
    AimFireCameraSettings = Definition.AimFireCameraSettings;
    AirborneCameraImpulseScale = Definition.AirborneCameraImpulseScale;
    AirborneCameraRecoveryScale = Definition.AirborneCameraRecoveryScale;
}
