#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/Core/Update/BBBMeleeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/BBBMeleeParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/BBBMeleeActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/AnimationSystem/BBBMeleeAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/NetworkSystem/BBBMeleeNetworkSystem.h"
void FBBBMeleeUpdatePipeline::UpdateInstance(ABBBEquipment &BaseEquipment) const
{
    auto &Equipment = static_cast<ABBBMeleeEquipment &>(BaseEquipment);
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    auto *Definition = Cast<UBBBMeleeDefinition>(Equipment.GetDefinition());
    auto *Mesh = Equipment.GetEquipmentSkeletalMesh();
    auto *World = Equipment.GetWorld();
    if (!ensureMsgf(Character && Definition && Mesh && World, TEXT("近战更新依赖无效")))
    {
        return;
    }
    FBBBMeleeUpdateContext Context{Equipment, *Character, *Mesh, *Definition, Equipment.RuntimeData, *World, !Equipment.IsMirror()};
    FBBBMeleeParseSystem::Update(Context);
    FBBBMeleeActionSystem::Update(Context);
    FBBBMeleeAnimationSystem::Update(Context);
    FBBBMeleeNetworkSystem::Update(Context, Character->HasNetworkAuthority());
}
