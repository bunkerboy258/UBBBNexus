#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/NetworkSystem/DomainData/BBBPistolNetworkDomainState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ParseSystem/DomainData/BBBPistolParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/BBBPistolActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/AnimationSystem/DomainData/BBBPistolAnimationDomainState.h"

/** 手枪唯一运行时聚合黑板 */
struct FBBBPistolRuntimeData final
{
    /** Parse 领域 */
    FBBBPistolParseDomainState Parse;

    /** Action 领域 */
    FBBBPistolActionDomainState Action;

    /** Animation 领域 */
    FBBBPistolAnimationDomainState Animation;

    /** 当前事实的网络观察基准 */
    FBBBPistolNetworkDomainState Network;
};
