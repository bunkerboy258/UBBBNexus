#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/BBBRevolverActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/Context/BBBRevolverActionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ActionSystem/Processors/BBBRevolverActionProcessor.h"

void FBBBRevolverActionSystem::Update(ABBBRevolverEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBRevolverDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBRevolverActionUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBRevolverActionProcessor::Update(Context);
}
