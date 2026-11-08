#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/NetworkSystem/BBBSniperNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/NetworkSystem/DomainData/Context/BBBSniperNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/NetworkSystem/Processors/BBBSniperNetworkProcessor.h"

void FBBBSniperNetworkSystem::Update(ABBBSniperEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    if (!(Character))
    {
        return;
    }
    FBBBSniperNetworkUpdateContext Context{Equipment, *Character, Equipment.RuntimeData, !Equipment.IsMirror()};

    const auto *Owner = Cast<ABBBCharacter>(Equipment.GetOwner());
    const bool bAuthority = Owner && Owner->HasNetworkAuthority();
    if (Context.bCausal && bAuthority)
    {
        FBBBSniperNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBSniperNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBSniperNetworkProcessor::Update(Context);
    }
}
