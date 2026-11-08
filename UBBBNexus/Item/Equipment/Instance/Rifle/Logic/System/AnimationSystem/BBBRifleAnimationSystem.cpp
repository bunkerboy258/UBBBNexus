#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/BBBRifleAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/DomainData/Context/BBBRifleAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRifleAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRiflePoseProcessor.h"

void FBBBRifleAnimationSystem::Update(ABBBRifleEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBRifleAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBRiflePoseProcessor::Update(Context);
    FBBBRifleAnimationProcessor::Update(Context);
}

void FBBBRifleAnimationSystem::Stop(ABBBRifleEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBRifleDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBRifleAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBRifleAnimationProcessor::Stop(Context);
}
