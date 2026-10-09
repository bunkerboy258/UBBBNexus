#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/NetworkSystem/BBBSMGNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/NetworkSystem/Processors/BBBSMGNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/Context/BBBSMGUpdateContext.h"

void FBBBSMGNetworkSystem::Update(FBBBSMGUpdateContext &Context, const bool bAuthority)
{
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
