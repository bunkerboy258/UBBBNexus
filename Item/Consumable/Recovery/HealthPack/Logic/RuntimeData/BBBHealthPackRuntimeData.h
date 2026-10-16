#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/DomainData/BBBHealthPackParseDomainState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/DomainData/BBBHealthPackUseDomainState.h"
/** 当次使用的唯一黑板 */
struct FBBBHealthPackRuntimeData final
{
    /** 固定输入领域 */
    FBBBHealthPackParseDomainState Parse;
    /** 使用领域 */
    FBBBHealthPackUseDomainState Use;
};
