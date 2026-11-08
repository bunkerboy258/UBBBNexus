#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/Core/Update/BBBSniperUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ParseSystem/BBBSniperParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ActionSystem/BBBSniperActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/BBBSniperAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/NetworkSystem/BBBSniperNetworkSystem.h"

void FBBBSniperUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSniperEquipment &>(BaseEquipment);
    FBBBSniperParseSystem::Update(Equipment);
    FBBBSniperActionSystem::Update(Equipment);
    FBBBSniperAnimationSystem::Update(Equipment);
    FBBBSniperNetworkSystem::Update(Equipment);
}
