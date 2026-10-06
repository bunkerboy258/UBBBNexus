#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterTraversalWarpProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "MotionWarpingComponent.h"

void FBBBCharacterTraversalWarpProcessor::Update(FBBBCharacterLocomotionUpdateContext &Context) const
{
    if (Context.Traversal.Action == EBBBTraversalAction::None || Context.Traversal.bEndRequested)
    {
        Context.Warping.RemoveWarpTarget(TEXT("TraversalContact"));
        Context.Warping.RemoveWarpTarget(TEXT("TraversalEnd"));
        return;
    }
    Context.Warping.AddOrUpdateWarpTargetFromTransform(TEXT("TraversalContact"), Context.Traversal.ContactTarget);
    Context.Warping.AddOrUpdateWarpTargetFromTransform(TEXT("TraversalEnd"), Context.Traversal.EndTarget);
}
