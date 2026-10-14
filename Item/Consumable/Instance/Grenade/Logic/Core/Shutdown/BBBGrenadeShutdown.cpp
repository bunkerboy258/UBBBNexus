#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Shutdown/BBBGrenadeShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/Processors/BBBGrenadeNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/Processors/BBBGrenadeParseProcessor.h"

void FBBBGrenadeShutdown::ShutdownInstance(ABBBConsumable &BaseActor) const
{
    auto &Actor = static_cast<ABBBGrenadeConsumable &>(BaseActor);
    // 投掷物独立存活 销毁手持实例不影响已释放的物理 Actor
    FBBBGrenadeUseProcessor::Clear(Actor.RuntimeData);
    FBBBGrenadeNetworkProcessor::Clear(Actor.RuntimeData);
    FBBBGrenadeParseProcessor::Clear(Actor.RuntimeData);
}
