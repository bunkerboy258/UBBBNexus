#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ParseSystem/BBBLMGParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ParseSystem/DomainData/Context/BBBLMGParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ParseSystem/Processors/BBBLMGParseProcessor.h"

void FBBBLMGParseSystem::Update(ABBBLMGEquipment &Equipment)
{
    FBBBLMGParseUpdateContext Context{Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBLMGParseProcessor::Update(Context);
}
