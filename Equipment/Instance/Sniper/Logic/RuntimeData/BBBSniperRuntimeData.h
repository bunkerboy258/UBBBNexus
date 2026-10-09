#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/NetworkSystem/DomainData/BBBSniperNetworkDomainState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ParseSystem/DomainData/BBBSniperParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/BBBSniperActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/DomainData/BBBSniperAnimationDomainState.h"

/** 狙击枪唯一运行时聚合黑板 */
struct FBBBSniperRuntimeData final
{
    /** Parse 领域 */
    FBBBSniperParseDomainState Parse;

    /** Action 领域 */
    FBBBSniperActionDomainState Action;

    /** Animation 领域 */
    FBBBSniperAnimationDomainState Animation;

    /** 当前事实的网络观察基准 */
    FBBBSniperNetworkDomainState Network;
};
