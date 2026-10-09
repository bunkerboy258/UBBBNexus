#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/NetworkSystem/DomainData/BBBRevolverNetworkDomainState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ParseSystem/DomainData/BBBRevolverParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/BBBRevolverActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/DomainData/BBBRevolverAnimationDomainState.h"

/** 左轮唯一运行时聚合黑板 */
struct FBBBRevolverRuntimeData final
{
    /** Parse 领域 */
    FBBBRevolverParseDomainState Parse;

    /** Action 领域 */
    FBBBRevolverActionDomainState Action;

    /** Animation 领域 */
    FBBBRevolverAnimationDomainState Animation;

    /** 当前事实的网络观察基准 */
    FBBBRevolverNetworkDomainState Network;
};
