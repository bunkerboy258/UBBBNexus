#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/NetworkSystem/BBBRevolverNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/NetworkSystem/DomainData/Context/BBBRevolverNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/NetworkSystem/Processors/BBBRevolverNetworkProcessor.h"

void FBBBRevolverNetworkSystem::Update(ABBBRevolverEquipment &Equipment)
{
    auto *Character = Cast<ABBBCharacter>(Equipment.GetOwner());
    if (!(Character))
    {
        return;
    }
    FBBBRevolverNetworkUpdateContext Context{Equipment, *Character, Equipment.RuntimeData, !Equipment.IsMirror()};

    const auto *Owner = Cast<ABBBCharacter>(Equipment.GetOwner());
    const bool bAuthority = Owner && Owner->HasNetworkAuthority();
    if (Context.bCausal && bAuthority)
    {
        FBBBRevolverNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBRevolverNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBRevolverNetworkProcessor::Update(Context);
    }
}
