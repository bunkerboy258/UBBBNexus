#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBAccelerationObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBAccelerationObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto &Observation = Context.Data.Network.AccelerationObservationState;
    const auto &Identity = Context.NetworkIdentityState;
    if (!Identity.bLocallyControlled)
    {
        // 主机分发收到的新结果 不以镜像 CMC 的加速度覆盖控制者事实
        const auto &Locomotion = Context.LocomotionState;
        if (Identity.bHasAuthority && Locomotion.AccelerationRevision > Observation.Revision)
        {
            Context.NetworkComponent.ReplicateAcceleration(
                Locomotion.AccelerationRevision, Locomotion.RestoredAcceleration,
                Locomotion.RestoredMovementInput);
            Observation.Revision = Locomotion.AccelerationRevision;
        }
        return;
    }

    // 动画领域上一晚更新已经采集 CMC 的真实结果 此处只读快照不生成移动事实
    const FVector Acceleration = Context.Data.Animation.ReadAnimationFactState().Acceleration;
    // 持续输入已由解析领域裁决 根运动接管时也不依赖 CMC 产生非零加速度
    const FVector MovementInput = Context.Data.Parse.ReadControlState().MoveWorld;
    const bool bChanged = Observation.Revision == 0
        || !Acceleration.Equals(Observation.LastObservedAcceleration, 0.1)
        || !MovementInput.Equals(Observation.LastObservedMovementInput, 0.001);
    if (Identity.bHasAuthority)
    {
        if (bChanged)
        {
            Observation.LastObservedAcceleration = Acceleration;
            Observation.LastObservedMovementInput = MovementInput;
            ++Observation.Revision;
            Context.NetworkComponent.ReplicateAcceleration(Observation.Revision, Acceleration, MovementInput);
        }
        return;
    }

    const double Now = Context.WorldState.WorldTimeSeconds;
    const double Interval = FMath::Max(Context.NetworkConfig.AccelerationUploadInterval, 0.016f);
    if (Observation.LastUploadTime >= 0.0 && Now - Observation.LastUploadTime < Interval)
    {
        return;
    }
    if (bChanged)
    {
        Observation.LastObservedAcceleration = Acceleration;
        Observation.LastObservedMovementInput = MovementInput;
        ++Observation.Revision;
    }

    // 重发同一当前版本可恢复丢包 松开移动后的零值也不会因一次丢包永久丢失
    Context.NetworkComponent.ServerSubmitAcceleration(Observation.Revision, Acceleration, MovementInput);
    Observation.LastUploadTime = Now;
}
