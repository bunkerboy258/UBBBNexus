#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterTraversalLifeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBTraversalConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBFullBodyMontageLocalControlPacket.h"

void FBBBCharacterTraversalLifeProcessor::Update(FBBBCharacterLocomotionUpdateContext &Context) const
{
    FBBBCharacterTraversalState &State = Context.Traversal;
    if (State.Action == EBBBTraversalAction::None)
    {
        return;
    }
    if (Context.Execution.bIsMirror)
    {
        /** 镜像只执行结束结果的播放清理 不重新决定动作生命周期 */
        if (State.bEndRequested && Context.AnimationFacts.bFullBodyPlaying
            && !Context.Montages.HasFullBodyRequest())
        {
            if (ABBBCharacter *Character = Cast<ABBBCharacter>(&Context.Character))
            {
                Character->SubmitInput(FBBBFullBodyMontageLocalControlPacket{});
            }
        }
        return;
    }
    if (Context.Montages.HasFullBodyRequest())
    {
        /** 新全身请求中断已开始的翻越 初始请求则允许接管根运动 */
        State.bEndRequested |= State.bPlaybackRequested;
        State.bPlaybackRequested |= Context.Montages.HasFullBodyAnimationRequest();
    }
    const bool bPlaying = Context.AnimationFacts.bFullBodyPlaying;
    const bool bEnded = State.bPlaybackObserved && !bPlaying;
    State.bPlaybackObserved |= State.bPlaybackRequested && bPlaying;
    const double Elapsed = Context.World.WorldTimeSeconds - State.StartTime;
    const bool bFailed = !State.bPlaybackObserved && Elapsed > Context.TraversalConfig.StartTimeout;
    const bool bFault = bFailed || Elapsed > Context.TraversalConfig.MaxDuration
        || !State.Obstacle.IsValid();
    if (bFault && bPlaying && !Context.Montages.HasFullBodyRequest())
    {
        /** 异常结束通过既有输入槽停止动画 不留下继续驱动角色的根运动 */
        if (ABBBCharacter *Character = Cast<ABBBCharacter>(&Context.Character))
        {
            Character->SubmitInput(FBBBFullBodyMontageLocalControlPacket{});
        }
    }
    State.bEndRequested |= bEnded || bFault;
}
