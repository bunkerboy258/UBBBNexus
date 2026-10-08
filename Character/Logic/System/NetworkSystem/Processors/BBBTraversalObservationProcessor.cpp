#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBTraversalObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBTraversalNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBTraversalObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    FBBBTraversalNetworkObservationState &State = Context.TraversalObservation;
    const ACharacter *Character = Cast<ACharacter>(Context.NetworkComponent.GetOwner());
    const UCharacterMovementComponent *Movement = Character ? Character->GetCharacterMovement() : nullptr;
    const FNetworkPredictionData_Client_Character *Prediction = nullptr;
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled
        && Movement && Movement->HasPredictionData_Client())
    {
        Prediction = Movement->GetPredictionData_Client_Character();
    }

    // 结束通知不能撤销途中已发送的纠正 交权后等待新的移动帧确认再恢复纠正
    const bool bTraversalPresent = Context.Traversal.Action != EBBBTraversalAction::None;
    if (!bTraversalPresent && State.bMovementCorrectionSuppressed && !State.bAwaitingExitMoveAck)
    {
        State.bAwaitingExitMoveAck = Prediction != nullptr;
        State.ExitMoveTimeStamp = Prediction ? Prediction->CurrentTimeStamp : 0.0f;
    }
    if (State.bAwaitingExitMoveAck && Prediction && Prediction->LastAckedMove.IsValid())
    {
        const bool bNewTimestampCycle = Prediction->CurrentTimeStamp < State.ExitMoveTimeStamp
            && !Prediction->LastAckedMove->bOldTimeStampBeforeReset;
        if (Prediction->LastAckedMove->TimeStamp > State.ExitMoveTimeStamp || bNewTimestampCycle)
        {
            State.bAwaitingExitMoveAck = false;
        }
    }
    if (bTraversalPresent || !Prediction)
    {
        State.bAwaitingExitMoveAck = false;
    }
    State.bMovementCorrectionSuppressed = bTraversalPresent || State.bAwaitingExitMoveAck;

    // 结束裁决先于物理交接发送 避免服务器仍用 Flying 校正已恢复移动的控制者
    const bool bActive = Context.Traversal.Action != EBBBTraversalAction::None && !Context.Traversal.bEndRequested;
    const EBBBTraversalAction Action = bActive ? Context.Traversal.Action : EBBBTraversalAction::None;
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
            Context.Traversal.PlaybackPosition, Context.LocomotionState.TraversalExitVelocity);
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitTraversal(Context.Traversal.ActionId,
            Action, Context.Traversal.ContactTarget, Context.Traversal.EndTarget,
            Context.Traversal.PlaybackPosition, Context.LocomotionState.TraversalExitVelocity);
    }
}
