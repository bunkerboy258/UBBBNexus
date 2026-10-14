#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/BBBSMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/DomainData/Context/BBBSMGAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGPoseProcessor.h"

void FBBBSMGAnimationSystem::Update(ABBBSMGEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBSMGDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBSMGAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};

    FBBBSMGPoseProcessor::Update(Context);
    FBBBSMGAnimationProcessor::Update(Context);
}

void FBBBSMGAnimationSystem::Stop(ABBBSMGEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *Definition = Cast<UBBBSMGDefinition>(Equipment.GetDefinition());
    auto *World = Equipment.GetWorld();
    if (!(Character && Mesh && Definition && World))
    {
        return;
    }
    FBBBSMGAnimationUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBSMGAnimationProcessor::Stop(Context);
}
