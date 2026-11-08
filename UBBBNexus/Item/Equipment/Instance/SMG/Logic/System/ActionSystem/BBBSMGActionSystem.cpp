#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/BBBSMGActionSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/Context/BBBSMGActionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ActionSystem/Processors/BBBSMGActionProcessor.h"

void FBBBSMGActionSystem::Update(ABBBSMGEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBSMGDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBSMGActionUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBSMGActionProcessor::Update(Context);
}
