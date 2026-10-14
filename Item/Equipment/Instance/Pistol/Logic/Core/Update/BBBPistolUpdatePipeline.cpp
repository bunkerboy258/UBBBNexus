#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/Core/Update/BBBPistolUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/BBBPistolParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/BBBPistolActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/BBBPistolAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/NetworkSystem/BBBPistolNetworkSystem.h"

void FBBBPistolUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBPistolEquipment &>(BaseEquipment);
    FBBBPistolParseSystem::Update(Equipment);
    FBBBPistolActionSystem::Update(Equipment);
    FBBBPistolAnimationSystem::Update(Equipment);
    FBBBPistolNetworkSystem::Update(Equipment);
}
