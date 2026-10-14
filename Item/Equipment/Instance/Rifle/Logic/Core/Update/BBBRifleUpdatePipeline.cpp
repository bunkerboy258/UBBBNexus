#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ParseSystem/BBBRifleParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ActionSystem/BBBRifleActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/BBBRifleAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"

void FBBBRifleUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBRifleEquipment &>(BaseEquipment);
    FBBBRifleParseSystem::Update(Equipment);
    FBBBRifleActionSystem::Update(Equipment);
    FBBBRifleAnimationSystem::Update(Equipment);
    FBBBRifleNetworkSystem::Update(Equipment);
}
