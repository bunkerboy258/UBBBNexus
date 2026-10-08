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

    /** 本次入场表现开始的世界时间 负值表示没有入场表现 */
    double DownedEntryStartTime = -1.0;
};
