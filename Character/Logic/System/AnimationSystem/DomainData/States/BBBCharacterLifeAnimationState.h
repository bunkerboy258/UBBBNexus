#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Definitions/BBBCharacterLifePhase.h"
#include "BBBCharacterLifeAnimationState.generated.h"

/** 已经提交到动画实例的生命阶段 */
USTRUCT()
struct FBBBCharacterLifeAnimationState final
{
    GENERATED_BODY()

    /** 最近应用的阶段 */
    EBBBCharacterLifePhase AppliedPhase = EBBBCharacterLifePhase::Alive;

    /** 最近应用的半蹲帮扶事实 */
    bool bAppliedRescueHelping = false;

    /** 最近应用的接受救援事实 */
    bool bAppliedRescueReceiving = false;

    /** 最近应用的救援操作 用于区分没有空帧间隔的重新救援 */
    uint64 AppliedRescueOperation = 0;
};
