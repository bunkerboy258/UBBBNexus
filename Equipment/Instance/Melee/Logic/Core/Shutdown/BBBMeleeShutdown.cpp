#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/Core/Shutdown/BBBMeleeShutdown.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/Processors/BBBMeleeParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/AnimationSystem/Processors/BBBMeleeAnimationProcessor.h"
void FBBBMeleeShutdown::ShutdownInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMeleeEquipment &>(BaseEquipment);
    FBBBMeleeActionProcessor::Stop(Equipment.RuntimeData);
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Definition = Cast<UBBBMeleeDefinition>(Equipment.GetDefinition());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *World = Equipment.GetWorld();
    if (Character && Definition && Mesh && World)
    {
        FBBBMeleeUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
        FBBBMeleeAnimationProcessor::Stop(Context);
    }
    FBBBMeleeParseProcessor::Clear(Equipment.RuntimeData);
}
