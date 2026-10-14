#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/Core/Update/BBBShotgunUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/BBBShotgunParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/BBBShotgunActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/BBBShotgunAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/NetworkSystem/BBBShotgunNetworkSystem.h"

void FBBBShotgunUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBShotgunEquipment &>(BaseEquipment);
    FBBBShotgunParseSystem::Update(Equipment);
    FBBBShotgunActionSystem::Update(Equipment);
    FBBBShotgunAnimationSystem::Update(Equipment);
    FBBBShotgunNetworkSystem::Update(Equipment);
}
