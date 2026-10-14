#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/BBBPistolParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/DomainData/Context/BBBPistolParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/Processors/BBBPistolParseProcessor.h"

void FBBBPistolParseSystem::Update(ABBBPistolEquipment &Equipment)
{
    FBBBPistolParseUpdateContext Context{Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBPistolParseProcessor::Update(Context);
}
