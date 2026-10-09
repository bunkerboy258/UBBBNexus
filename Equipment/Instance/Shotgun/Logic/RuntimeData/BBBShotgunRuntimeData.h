#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/NetworkSystem/DomainData/BBBShotgunNetworkDomainState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ParseSystem/DomainData/BBBShotgunParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/BBBShotgunActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/DomainData/BBBShotgunAnimationDomainState.h"

/** 霰弹枪唯一运行时聚合黑板 */
struct FBBBShotgunRuntimeData final
{
    /** Parse 领域 */
    FBBBShotgunParseDomainState Parse;

    /** Action 领域 */
    FBBBShotgunActionDomainState Action;

    /** Animation 领域 */
    FBBBShotgunAnimationDomainState Animation;

    /** 当前事实的网络观察基准 */
    FBBBShotgunNetworkDomainState Network;
};
