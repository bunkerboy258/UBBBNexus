#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBLocomotionObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBLocomotionNetworkObservationState.h"

void FBBBLocomotionObservationProcessor::Update(
    FBBBCharacterNetworkUpdateContext &Context) const
{
    if (Context.LocomotionObservationState.LastObservedGait.IsSet()
        && Context.LocomotionObservationState.LastObservedGait.GetValue()
        == Context.LocomotionState.Gait)
    {
        return;
    }

    Context.LocomotionObservationState.LastObservedGait = Context.LocomotionState.Gait;

    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateLocomotionState(Context.LocomotionState.Gait);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitLocomotionState(Context.LocomotionState.Gait);
    }
}
