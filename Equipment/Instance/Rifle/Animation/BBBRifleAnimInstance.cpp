#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Animation/BBBRifleAnimInstance.h"

void UBBBRifleAnimInstance::PublishRifleSnapshot(
    const int32 InLoadedAmmo,
    const int32 InAmmoCapacity,
    const bool bInReloading,
    const float InTimeSinceLastFireSeconds)
{
    // 游戏线程统一发布 动画图不再读取装备可变黑板
    check(IsInGameThread());
    LoadedAmmo = InLoadedAmmo;
    AmmoCapacity = InAmmoCapacity;
    this->bIsReloading = bInReloading;
    TimeSinceLastFireSeconds = InTimeSinceLastFireSeconds;
}
