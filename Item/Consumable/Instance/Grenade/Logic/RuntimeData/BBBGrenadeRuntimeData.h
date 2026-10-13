#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/BBBGrenadeParseDomainState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/BBBGrenadeUseDomainState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/DomainData/BBBGrenadeNetworkDomainState.h"

/** 手雷唯一聚合黑板 */
struct FBBBGrenadeRuntimeData final
{
    FBBBGrenadeParseDomainState Parse;
    FBBBGrenadeUseDomainState Use;
    FBBBGrenadeNetworkDomainState Network;
};
