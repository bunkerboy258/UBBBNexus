#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/Core/Update/BBBLMGUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ParseSystem/BBBLMGParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/BBBLMGActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/BBBLMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/NetworkSystem/BBBLMGNetworkSystem.h"

void FBBBLMGUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBLMGEquipment &>(BaseEquipment);
    FBBBLMGParseSystem::Update(Equipment);
    FBBBLMGActionSystem::Update(Equipment);
    FBBBLMGAnimationSystem::Update(Equipment);
    FBBBLMGNetworkSystem::Update(Equipment);
}
