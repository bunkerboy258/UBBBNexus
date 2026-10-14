#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/AnimationSystem/BBBMinigunAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Config/BBBMinigunDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/AnimationSystem/DomainData/Context/BBBMinigunAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/AnimationSystem/Processors/BBBMinigunAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/AnimationSystem/Processors/BBBMinigunPoseProcessor.h"

void FBBBMinigunAnimationSystem::Update(ABBBMinigunEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBMinigunDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBMinigunAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBMinigunPoseProcessor::Update(Context);
    FBBBMinigunAnimationProcessor::Update(Context);
}

void FBBBMinigunAnimationSystem::Stop(ABBBMinigunEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBMinigunDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBMinigunAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBMinigunAnimationProcessor::Stop(Context);
}
