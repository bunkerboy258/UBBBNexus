#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/AnimationSystem/BBBRevolverAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/AnimationSystem/DomainData/Context/BBBRevolverAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/AnimationSystem/Processors/BBBRevolverAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/AnimationSystem/Processors/BBBRevolverPoseProcessor.h"

void FBBBRevolverAnimationSystem::Update(ABBBRevolverEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBRevolverDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBRevolverAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBRevolverPoseProcessor::Update(Context);
    FBBBRevolverAnimationProcessor::Update(Context);
}

void FBBBRevolverAnimationSystem::Stop(ABBBRevolverEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBRevolverDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBRevolverAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBRevolverAnimationProcessor::Stop(Context);
}
