#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/BBBShotgunParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/DomainData/Context/BBBShotgunParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/Processors/BBBShotgunParseProcessor.h"

void FBBBShotgunParseSystem::Update(ABBBShotgunEquipment &Equipment)
{
    FBBBShotgunParseUpdateContext Context{Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBShotgunParseProcessor::Update(Context);
}
