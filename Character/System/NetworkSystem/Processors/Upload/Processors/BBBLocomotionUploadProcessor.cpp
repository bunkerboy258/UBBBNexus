#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBLocomotionUploadProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

void FBBBLocomotionUploadProcessor::Update(
    const FBBBCharacterLocomotionRuntimeData &LocomotionData,
    FBBBNetworkRuntimeData &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    const FBBBLocomotionNetworkObserverState &PreviousObserverState =
        NetworkData.GetLocomotionObserverState();
    const EBBBCharacterGait CurrentGait = LocomotionData.GetGait();

    if (PreviousObserverState.LastObservedState.IsSet()
        && PreviousObserverState.LastObservedState->Gait == CurrentGait)
    {
        return;
    }

    FBBBLocomotionNetworkState State;
    State.Gait = CurrentGait;
    NetworkSystem.SubmitLocomotionState(State);

    FBBBLocomotionNetworkObserverState ObserverState;
    ObserverState.LastObservedState = State;
    NetworkData.CommitLocomotionObserverState(ObserverState);
}
