#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/BBBRifleParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/BBBRifleActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/DomainData/BBBRifleAnimationDomainState.h"

/** 步枪唯一运行时聚合黑板 */
struct FBBBRifleRuntimeData final
{
    /** Parse 领域 */
    FBBBRifleParseDomainState Parse;

    /** Action 领域 */
    FBBBRifleActionDomainState Action;

    /** Animation 领域 */
    FBBBRifleAnimationDomainState Animation;

};
