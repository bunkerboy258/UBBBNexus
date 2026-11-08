#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/DomainData/BBBGrenadeParseDomainState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/BBBGrenadeUseDomainState.h"
/** 当次使用的唯一黑板 */
struct FBBBGrenadeRuntimeData final
{
    /** 固定输入领域 */
    FBBBGrenadeParseDomainState Parse;
    /** 使用领域 */
    FBBBGrenadeUseDomainState Use;
};
