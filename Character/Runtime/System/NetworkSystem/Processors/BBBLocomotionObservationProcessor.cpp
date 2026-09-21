#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBLocomotionObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBCharacterNetworkState.h"

void FBBBLocomotionObservationProcessor::Update(
    FBBBCharacterNetworkUpdateContext &Context) const
{
    if (Context.NetworkState.LastObservedLocomotion.IsSet()
        && Context.NetworkState.LastObservedLocomotion->Gait == Context.LocomotionState.Gait)
    {
        return;
    }

    FBBBLocomotionNetworkPayload Payload;
    Payload.Gait = Context.LocomotionState.Gait;
    Context.NetworkState.LastObservedLocomotion = Payload;

    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateLocomotionState(Payload);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitLocomotionState(Payload);
    }
}
