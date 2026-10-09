#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/NetworkSystem/BBBPistolNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/NetworkSystem/Processors/BBBPistolNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/Context/BBBPistolUpdateContext.h"

void FBBBPistolNetworkSystem::Update(FBBBPistolUpdateContext &Context, const bool bAuthority)
{
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
