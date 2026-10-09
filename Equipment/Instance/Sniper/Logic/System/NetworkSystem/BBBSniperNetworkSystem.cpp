#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/NetworkSystem/BBBSniperNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/NetworkSystem/Processors/BBBSniperNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/Context/BBBSniperUpdateContext.h"

void FBBBSniperNetworkSystem::Update(FBBBSniperUpdateContext &Context, const bool bAuthority)
{
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
