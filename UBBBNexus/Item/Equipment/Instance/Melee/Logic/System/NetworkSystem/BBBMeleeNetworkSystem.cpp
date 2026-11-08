#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/NetworkSystem/BBBMeleeNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/NetworkSystem/DomainData/Context/BBBMeleeNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/NetworkSystem/Processors/BBBMeleeNetworkProcessor.h"

void FBBBMeleeNetworkSystem::Update(ABBBMeleeEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    if (!(Character))
    {
        return;
    }
    FBBBMeleeNetworkUpdateContext Context{Equipment, *Character, Equipment.RuntimeData, !Equipment.IsMirror()};

    const auto *Owner = Cast<ABBBCharacter>(Equipment.GetOwner());
    const bool bAuthority = Owner && Owner->HasNetworkAuthority();
    if (Context.bCausal && bAuthority)
    {
        FBBBMeleeNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBMeleeNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBMeleeNetworkProcessor::Update(Context);
    }
}
