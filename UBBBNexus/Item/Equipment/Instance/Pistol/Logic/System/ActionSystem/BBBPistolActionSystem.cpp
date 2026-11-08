#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/BBBPistolActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/Context/BBBPistolActionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ActionSystem/Processors/BBBPistolActionProcessor.h"

void FBBBPistolActionSystem::Update(ABBBPistolEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBPistolDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBPistolActionUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBPistolActionProcessor::Update(Context);
}
