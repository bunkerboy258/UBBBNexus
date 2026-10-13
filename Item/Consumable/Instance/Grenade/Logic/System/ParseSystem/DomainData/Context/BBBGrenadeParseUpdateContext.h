#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/States/BBBGrenadeInputState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"

/** 手雷输入解析的一次调用数据 */
struct FBBBGrenadeParseUpdateContext final
{
    FBBBGrenadeInputState &Slots;
    FBBBGrenadeUseInputState &Inputs;
    FBBBGrenadeUseState &State;
    bool bIsMirror = true;
};
