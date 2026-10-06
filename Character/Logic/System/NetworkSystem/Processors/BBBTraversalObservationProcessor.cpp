#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBTraversalObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBTraversalNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBTraversalObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    const bool bActive = Context.Traversal.Action != EBBBTraversalAction::None;
    FBBBTraversalNetworkObservationState &State = Context.TraversalObservation;
    if (State.LastActionId == Context.Traversal.ActionId && State.bLastActive == bActive)
    {
        return;
    }
    State.LastActionId = Context.Traversal.ActionId;
    State.bLastActive = bActive;
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateTraversal(Context.Traversal.ActionId,
            Context.Traversal.Action, Context.Traversal.ContactTarget, Context.Traversal.EndTarget);
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitTraversal(Context.Traversal.ActionId,
            Context.Traversal.Action, Context.Traversal.ContactTarget, Context.Traversal.EndTarget);
    }
}
