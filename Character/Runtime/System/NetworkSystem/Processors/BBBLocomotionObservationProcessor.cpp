#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBLocomotionObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Context/BBBCharacterNetworkObservationContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkState.h"

void FBBBLocomotionObservationProcessor::Update(
    const FBBBCharacterLocomotionRuntimeData &LocomotionData,
    FBBBNetworkState &NetworkData,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    // 读取上次步态观察结果判断是否发生变化
    FBBBLocomotionObservationContext Context;
    Context.Gait = LocomotionData.Gait;

    if (NetworkData.LocomotionObserverState.LastObservedState.IsSet()
        && NetworkData.LocomotionObserverState.LastObservedState->Gait == Context.Gait)
    {
        // 步态未变化时不重复提交网络状态
        return;
    }

    // 将当前步态封装为网络状态并提交
    Context.State.Gait = Context.Gait;
    NetworkSystem.TransmitLocomotionState(Context.State);

    Context.Observer.LastObservedState = Context.State;
    // 保存最新观察结果供下一帧比较
    NetworkData.LocomotionObserverState = Context.Observer;
}
