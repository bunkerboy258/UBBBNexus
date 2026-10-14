#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/Core/Update/BBBMinigunUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ParseSystem/BBBMinigunParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ActionSystem/BBBMinigunActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/AnimationSystem/BBBMinigunAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/NetworkSystem/BBBMinigunNetworkSystem.h"

void FBBBMinigunUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMinigunEquipment &>(BaseEquipment);
    FBBBMinigunParseSystem::Update(Equipment);
    FBBBMinigunActionSystem::Update(Equipment);
    FBBBMinigunAnimationSystem::Update(Equipment);
    FBBBMinigunNetworkSystem::Update(Equipment);
}
