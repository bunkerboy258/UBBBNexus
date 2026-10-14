#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/BBBPistolAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/DomainData/Context/BBBPistolAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/Processors/BBBPistolAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/AnimationSystem/Processors/BBBPistolPoseProcessor.h"

void FBBBPistolAnimationSystem::Update(ABBBPistolEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBPistolDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBPistolAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBPistolPoseProcessor::Update(Context);
    FBBBPistolAnimationProcessor::Update(Context);
}

void FBBBPistolAnimationSystem::Stop(ABBBPistolEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBPistolDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBPistolAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBPistolAnimationProcessor::Stop(Context);
}
