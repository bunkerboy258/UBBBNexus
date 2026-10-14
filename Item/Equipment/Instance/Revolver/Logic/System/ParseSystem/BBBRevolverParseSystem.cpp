#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ParseSystem/BBBRevolverParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ParseSystem/DomainData/Context/BBBRevolverParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ParseSystem/Processors/BBBRevolverParseProcessor.h"

void FBBBRevolverParseSystem::Update(ABBBRevolverEquipment &Equipment)
{
    FBBBRevolverParseUpdateContext Context{Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBRevolverParseProcessor::Update(Context);
}
