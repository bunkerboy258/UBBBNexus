#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/NetworkSystem/DomainData/BBBMinigunNetworkDomainState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ParseSystem/DomainData/BBBMinigunParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/BBBMinigunActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/DomainData/BBBMinigunAnimationDomainState.h"

/** 转管机枪唯一运行时聚合黑板 */
struct FBBBMinigunRuntimeData final
{
    /** Parse 领域 */
    FBBBMinigunParseDomainState Parse;

    /** Action 领域 */
    FBBBMinigunActionDomainState Action;

    /** Animation 领域 */
    FBBBMinigunAnimationDomainState Animation;

    /** 当前事实的网络观察基准 */
    FBBBMinigunNetworkDomainState Network;
};
