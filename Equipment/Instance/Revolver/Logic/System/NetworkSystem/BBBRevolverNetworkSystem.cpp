#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/NetworkSystem/BBBRevolverNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/NetworkSystem/Processors/BBBRevolverNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/Context/BBBRevolverUpdateContext.h"

void FBBBRevolverNetworkSystem::Update(FBBBRevolverUpdateContext &Context, const bool bAuthority)
{
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
