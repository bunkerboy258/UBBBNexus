#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/AnimationSystem/BBBMeleeAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/AnimationSystem/DomainData/Context/BBBMeleeAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/AnimationSystem/Processors/BBBMeleeAnimationProcessor.h"
void FBBBMeleeAnimationSystem::Update(ABBBMeleeEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Definition = Cast<UBBBMeleeDefinition>(Equipment.GetDefinition());
    if (!(Character && Definition))
    {
        return;
    }
    FBBBMeleeAnimationUpdateContext Context{Equipment, *Character, *Definition, Equipment.RuntimeData, !Equipment.IsMirror()};

    FBBBMeleeAnimationProcessor::Update(Context);
}

void FBBBMeleeAnimationSystem::Stop(ABBBMeleeEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Definition = Cast<UBBBMeleeDefinition>(Equipment.GetDefinition());
    if (!(Character && Definition))
    {
        return;
    }
    FBBBMeleeAnimationUpdateContext Context{Equipment, *Character, *Definition, Equipment.RuntimeData, !Equipment.IsMirror()};
    FBBBMeleeAnimationProcessor::Stop(Context);
}
