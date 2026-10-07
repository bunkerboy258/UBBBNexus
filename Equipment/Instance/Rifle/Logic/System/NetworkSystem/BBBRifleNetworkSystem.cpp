#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/Processors/BBBRifleNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/Context/BBBRifleUpdateContext.h"

void FBBBRifleNetworkSystem::Update(FBBBRifleUpdateContext &Context, const bool bAuthority)
{
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
