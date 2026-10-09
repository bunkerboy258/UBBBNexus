#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/NetworkSystem/DomainData/BBBSMGNetworkDomainState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ParseSystem/DomainData/BBBSMGParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/BBBSMGActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/DomainData/BBBSMGAnimationDomainState.h"

/** 冲锋枪唯一运行时聚合黑板 */
struct FBBBSMGRuntimeData final
{
    /** Parse 领域 */
    FBBBSMGParseDomainState Parse;

    /** Action 领域 */
    FBBBSMGActionDomainState Action;

    /** Animation 领域 */
    FBBBSMGAnimationDomainState Animation;

    /** 当前事实的网络观察基准 */
    FBBBSMGNetworkDomainState Network;
};
