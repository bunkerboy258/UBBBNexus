#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/BBBSniperAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/DomainData/Context/BBBSniperAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperPoseProcessor.h"

void FBBBSniperAnimationSystem::Update(ABBBSniperEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBSniperDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBSniperAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBSniperPoseProcessor::Update(Context);
    FBBBSniperAnimationProcessor::Update(Context);
}

void FBBBSniperAnimationSystem::Stop(ABBBSniperEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBSniperDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBSniperAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBSniperAnimationProcessor::Stop(Context);
}
