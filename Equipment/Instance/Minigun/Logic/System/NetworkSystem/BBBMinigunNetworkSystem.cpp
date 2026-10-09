#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/NetworkSystem/BBBMinigunNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/NetworkSystem/Processors/BBBMinigunNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/Context/BBBMinigunUpdateContext.h"

void FBBBMinigunNetworkSystem::Update(FBBBMinigunUpdateContext &Context, const bool bAuthority)
{
    if (Context.bCausal && bAuthority)
    {
        FBBBMinigunNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBMinigunNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBMinigunNetworkProcessor::Update(Context);
    }
}
