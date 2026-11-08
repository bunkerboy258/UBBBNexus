#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/NetworkSystem/BBBSMGNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/NetworkSystem/DomainData/Context/BBBSMGNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/NetworkSystem/Processors/BBBSMGNetworkProcessor.h"

void FBBBSMGNetworkSystem::Update(ABBBSMGEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    if (!(Character))
    {
        return;
    }
    FBBBSMGNetworkUpdateContext Context{Equipment, *Character, Equipment.RuntimeData, !Equipment.IsMirror()};

    const auto *Owner = Cast<ABBBCharacter>(Equipment.GetOwner());
    const bool bAuthority = Owner && Owner->HasNetworkAuthority();
    if (Context.bCausal && bAuthority)
    {
        FBBBSMGNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBSMGNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBSMGNetworkProcessor::Update(Context);
    }
}
