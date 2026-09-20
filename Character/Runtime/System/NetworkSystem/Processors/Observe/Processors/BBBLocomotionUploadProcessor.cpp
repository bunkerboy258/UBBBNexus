#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/Observe/Processors/BBBLocomotionUploadProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkState.h"

void FBBBLocomotionUploadProcessor::Update(
    const FBBBCharacterLocomotionRuntimeData &LocomotionData,
    FBBBNetworkState &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    // 读取上次步态观察结果判断是否发生变化
    const FBBBLocomotionNetworkObserverState &PreviousObserverState =
        NetworkData.GetLocomotionObserverState();
    const EBBBCharacterGait CurrentGait = LocomotionData.GetGait();

    if (PreviousObserverState.LastObservedState.IsSet()
        && PreviousObserverState.LastObservedState->Gait == CurrentGait)
    {
        // 步态未变化时不重复提交网络状态
        return;
    }

    // 将当前步态封装为网络状态并提交
    FBBBLocomotionNetworkState State;
    State.Gait = CurrentGait;
    NetworkSystem.SubmitLocomotionState(State);

    FBBBLocomotionNetworkObserverState ObserverState;
    ObserverState.LastObservedState = State;
    // 保存最新观察结果供下一帧比较
    NetworkData.CommitLocomotionObserverState(ObserverState);
}
