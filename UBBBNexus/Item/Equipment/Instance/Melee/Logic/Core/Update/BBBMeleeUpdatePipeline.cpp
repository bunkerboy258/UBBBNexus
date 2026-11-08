#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/Core/Update/BBBMeleeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ParseSystem/BBBMeleeParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/BBBMeleeActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/AnimationSystem/BBBMeleeAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/NetworkSystem/BBBMeleeNetworkSystem.h"

void FBBBMeleeUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMeleeEquipment &>(BaseEquipment);
    FBBBMeleeParseSystem::Update(Equipment);
    FBBBMeleeActionSystem::Update(Equipment);
    FBBBMeleeAnimationSystem::Update(Equipment);
    FBBBMeleeNetworkSystem::Update(Equipment);
}
