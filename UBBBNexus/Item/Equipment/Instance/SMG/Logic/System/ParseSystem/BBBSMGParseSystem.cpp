#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ParseSystem/BBBSMGParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ParseSystem/DomainData/Context/BBBSMGParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ParseSystem/Processors/BBBSMGParseProcessor.h"

void FBBBSMGParseSystem::Update(ABBBSMGEquipment &Equipment)
{
    FBBBSMGParseUpdateContext Context{Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBSMGParseProcessor::Update(Context);
}
