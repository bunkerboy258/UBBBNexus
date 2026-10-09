#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/NetworkSystem/DomainData/BBBLMGNetworkDomainState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ParseSystem/DomainData/BBBLMGParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/BBBLMGActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/AnimationSystem/DomainData/BBBLMGAnimationDomainState.h"

/** 轻机枪唯一运行时聚合黑板 */
struct FBBBLMGRuntimeData final
{
    /** Parse 领域 */
    FBBBLMGParseDomainState Parse;

    /** Action 领域 */
    FBBBLMGActionDomainState Action;

    /** Animation 领域 */
    FBBBLMGAnimationDomainState Animation;

    /** 当前事实的网络观察基准 */
    FBBBLMGNetworkDomainState Network;
};
