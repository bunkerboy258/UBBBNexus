#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/BBBLMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/BBBLMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Config/BBBLMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/DomainData/Context/BBBLMGAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/Processors/BBBLMGAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/AnimationSystem/Processors/BBBLMGPoseProcessor.h"

void FBBBLMGAnimationSystem::Update(ABBBLMGEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBLMGDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBLMGAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBLMGPoseProcessor::Update(Context);
    FBBBLMGAnimationProcessor::Update(Context);
}

void FBBBLMGAnimationSystem::Stop(ABBBLMGEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBLMGDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBLMGAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBLMGAnimationProcessor::Stop(Context);
}
