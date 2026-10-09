#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/NetworkSystem/BBBShotgunNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/NetworkSystem/Processors/BBBShotgunNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/Context/BBBShotgunUpdateContext.h"

void FBBBShotgunNetworkSystem::Update(FBBBShotgunUpdateContext &Context, const bool bAuthority)
{
    if (Context.bCausal && bAuthority)
    {
        FBBBShotgunNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBShotgunNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBShotgunNetworkProcessor::Update(Context);
    }
}
