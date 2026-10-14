#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ParseSystem/BBBRifleParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/Context/BBBRifleParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"

void FBBBRifleParseSystem::Update(ABBBRifleEquipment &Equipment)
{
    FBBBRifleParseUpdateContext Context{Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBRifleParseProcessor::Update(Context);
}
