#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ParseSystem/BBBMinigunParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ParseSystem/DomainData/Context/BBBMinigunParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ParseSystem/Processors/BBBMinigunParseProcessor.h"

void FBBBMinigunParseSystem::Update(ABBBMinigunEquipment &Equipment)
{
    FBBBMinigunParseUpdateContext Context{Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBMinigunParseProcessor::Update(Context);
}
