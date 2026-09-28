#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimationFacts.h"

class UStaticMeshComponent;

/** 动画发布快照与已经表现的动作结果 */
struct FBBBRifleAnimationState final
{
    /** 游戏线程计算的握持与瞄准目标 */
    FBBBEquipmentAnimationFacts Pose;

    /** 是否已经发布初始快照 */
    bool bInitialized = false;

    /** 最近已经表现的开火次数 */
    int32 FireSequence = 0;

    /** 最近已经表现的换弹序号 */
    int32 ReloadSequence = 0;

    /** 最近已经表现的换弹状态 */
    bool bIsReloading = false;

    /** 当前由左手持有的临时弹匣 */
    TWeakObjectPtr<UStaticMeshComponent> HandMagazine;

    /** 最近已经表现的弹匣脱离状态 */
    bool bWasMagazineDetached = false;

    /** 最近已经表现的旧弹匣甩出状态 */
    bool bWasMagazineReleased = false;

    /** 最近已经表现的新弹匣手持状态 */
    bool bWasFreshMagazineHeld = false;

    /** 前一帧左手位置用于计算甩出速度 */
    FVector PreviousMagazineHandLocation = FVector::ZeroVector;

    /** 前一帧左手位置是否可用于甩出速度 */
    bool bHasPreviousMagazineHandLocation = false;

private:
    friend struct FBBBRifleAnimationDomainState;
    FBBBRifleAnimationState() = default;
};
