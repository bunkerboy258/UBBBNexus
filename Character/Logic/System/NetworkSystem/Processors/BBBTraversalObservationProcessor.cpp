#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBTraversalObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBTraversalNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBTraversalObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    // 结束裁决先于物理交接发送 避免服务器仍用 Flying 校正已恢复移动的控制者
    const bool bActive = Context.Traversal.Action != EBBBTraversalAction::None && !Context.Traversal.bEndRequested;
    const EBBBTraversalAction Action = bActive ? Context.Traversal.Action : EBBBTraversalAction::None;
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
            Action, Context.Traversal.ContactTarget, Context.Traversal.EndTarget,
            Context.Traversal.PlaybackPosition);
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitTraversal(Context.Traversal.ActionId,
            Action, Context.Traversal.ContactTarget, Context.Traversal.EndTarget,
            Context.Traversal.PlaybackPosition);
    }
}
