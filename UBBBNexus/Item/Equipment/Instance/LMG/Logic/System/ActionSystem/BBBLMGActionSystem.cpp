#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/BBBLMGActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/Context/BBBLMGActionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ActionSystem/Processors/BBBLMGActionProcessor.h"

void FBBBLMGActionSystem::Update(ABBBLMGEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBLMGDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBLMGActionUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBLMGActionProcessor::Update(Context);
}
