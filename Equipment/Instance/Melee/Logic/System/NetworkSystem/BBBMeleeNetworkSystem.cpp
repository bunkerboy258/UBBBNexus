#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/NetworkSystem/BBBMeleeNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/NetworkSystem/Processors/BBBMeleeNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"

void FBBBMeleeNetworkSystem::Update(FBBBMeleeUpdateContext &Context, const bool bAuthority)
{
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
