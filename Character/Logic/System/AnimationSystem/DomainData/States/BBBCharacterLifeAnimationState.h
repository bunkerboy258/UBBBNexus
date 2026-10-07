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
};
