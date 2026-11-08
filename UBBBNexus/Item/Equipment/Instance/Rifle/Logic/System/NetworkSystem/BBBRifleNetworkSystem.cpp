#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/NetworkSystem/DomainData/Context/BBBRifleNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/NetworkSystem/Processors/BBBRifleNetworkProcessor.h"

void FBBBRifleNetworkSystem::Update(ABBBRifleEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    if (!(Character))
    {
        return;
    }
    FBBBRifleNetworkUpdateContext Context{Equipment, *Character, Equipment.RuntimeData, !Equipment.IsMirror()};

    const auto *Owner = Cast<ABBBCharacter>(Equipment.GetOwner());
    const bool bAuthority = Owner && Owner->HasNetworkAuthority();
    if (Context.bCausal && bAuthority)
    {
        FBBBRifleNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBRifleNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBRifleNetworkProcessor::Update(Context);
    }
}
