#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentAnimationFacts.h"

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

private:
    friend struct FBBBRifleAnimationDomainState;
    FBBBRifleAnimationState() = default;
};
