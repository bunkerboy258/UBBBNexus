#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Animation/BBBRifleAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionState.h"

void UBBBRifleAnimInstance::PublishRifleState(const FBBBRifleActionState &State, const float WorldTime)
{
    // 游戏线程统一发布 动画图不再读取装备可变黑板
    check(IsInGameThread());
    LoadedAmmo = State.LoadedAmmo;
    AmmoCapacity = State.AmmoCapacity;
    bIsReloading = State.bIsReloading;
    TimeSinceLastFireSeconds = WorldTime - State.LastFireTimeSeconds;
}
