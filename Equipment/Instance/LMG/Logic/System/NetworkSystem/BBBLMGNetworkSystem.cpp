#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/NetworkSystem/BBBLMGNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/NetworkSystem/Processors/BBBLMGNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/Context/BBBLMGUpdateContext.h"

void FBBBLMGNetworkSystem::Update(FBBBLMGUpdateContext &Context, const bool bAuthority)
{
    if (Context.bCausal && bAuthority)
    {
        FBBBLMGNetworkProcessor::Update(Context);
        return;
    }

    if (Context.bCausal && !bAuthority)
    {
        FBBBLMGNetworkProcessor::Update(Context);
        return;
    }

    if (!Context.bCausal && bAuthority)
    {
        FBBBLMGNetworkProcessor::Update(Context);
    }
}
