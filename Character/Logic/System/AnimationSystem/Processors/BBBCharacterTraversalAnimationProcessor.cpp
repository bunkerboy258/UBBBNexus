#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterTraversalAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"

void FBBBCharacterTraversalAnimationProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    const FBBBCharacterTraversalState &Traversal = Context.RuntimeData.Locomotion.ReadTraversalState();
    FBBBCharacterTraversalAnimationState &Dispatch = Context.RuntimeData.Animation.TraversalAnimationState;
    if (Traversal.Action == EBBBTraversalAction::None || Traversal.bEndRequested
        || Traversal.ActionId == Dispatch.LastActionId)
    {
        return;
    }
    UBBBAnimInstance *Layer = Cast<UBBBAnimInstance>(Context.AnimationInstance.GetLinkedAnimLayerInstanceByClass(
        Context.AnimationLayerState.LinkedAnimationLayerClass));
    if (!Layer)
    {
        return;
    }
    Dispatch.LastActionId = Traversal.ActionId;
    Layer->TraversalRequested(Traversal.Action);
}
