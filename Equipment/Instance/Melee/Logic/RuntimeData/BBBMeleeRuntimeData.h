#pragma once
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/NetworkSystem/DomainData/BBBMeleeNetworkDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/DomainData/BBBMeleeParseDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/BBBMeleeActionDomainState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/AnimationSystem/DomainData/BBBMeleeAnimationDomainState.h"
/** 近战唯一运行时聚合黑板 */
struct FBBBMeleeRuntimeData final
{
    /** 固定输入队列 */
    FBBBMeleeParseDomainState Parse;
    /** 攻击因果结果 */
    FBBBMeleeActionDomainState Action;
    /** 表现消费结果 */
    FBBBMeleeAnimationDomainState Animation;
    /** 当前事实的网络观察基准 */
    FBBBMeleeNetworkDomainState Network;
};
