#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/Core/Update/BBBRevolverUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ParseSystem/BBBRevolverParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/BBBRevolverActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/AnimationSystem/BBBRevolverAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/NetworkSystem/BBBRevolverNetworkSystem.h"

void FBBBRevolverUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRevolverEquipment &>(BaseEquipment);
    FBBBRevolverParseSystem::Update(Equipment);
    FBBBRevolverActionSystem::Update(Equipment);
    FBBBRevolverAnimationSystem::Update(Equipment);
    FBBBRevolverNetworkSystem::Update(Equipment);
}
