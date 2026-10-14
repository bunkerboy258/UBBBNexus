#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/Core/Update/BBBSMGUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ParseSystem/BBBSMGParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/BBBSMGActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/BBBSMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/NetworkSystem/BBBSMGNetworkSystem.h"

void FBBBSMGUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBSMGEquipment &>(BaseEquipment);
    FBBBSMGParseSystem::Update(Equipment);
    FBBBSMGActionSystem::Update(Equipment);
    FBBBSMGAnimationSystem::Update(Equipment);
    FBBBSMGNetworkSystem::Update(Equipment);
}
