#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/BBBMeleeActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeActionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeActionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeContactProcessor.h"
void FBBBMeleeActionSystem::Update(ABBBMeleeEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBMeleeDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBMeleeActionUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBMeleeActionProcessor::Update(Context);
    FBBBMeleeContactProcessor::Update(Context);
}
