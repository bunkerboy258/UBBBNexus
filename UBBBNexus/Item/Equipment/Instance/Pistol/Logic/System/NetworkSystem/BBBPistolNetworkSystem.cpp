#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/NetworkSystem/BBBPistolNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/BBBPistolEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Config/BBBPistolDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/NetworkSystem/DomainData/Context/BBBPistolNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/NetworkSystem/Processors/BBBPistolNetworkProcessor.h"

void FBBBPistolNetworkSystem::Update(ABBBPistolEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    if (!(Character))
    {
        return;
    }
    FBBBPistolNetworkUpdateContext Context{Equipment, *Character, Equipment.RuntimeData, !Equipment.IsMirror()};

    const auto *Owner = Cast<ABBBCharacter>(Equipment.GetOwner());
    const bool bAuthority = Owner && Owner->HasNetworkAuthority();
    if (Context.bCausal && bAuthority)
    {
        FBBBPistolNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBPistolNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBPistolNetworkProcessor::Update(Context);
    }
}
