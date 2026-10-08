#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/Processors/BBBCharacterTraversalLifeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/Context/BBBCharacterTraversalUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBTraversalConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterTraversalAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"

void FBBBCharacterTraversalLifeProcessor::Update(FBBBCharacterTraversalUpdateContext &Context) const
{
    FBBBCharacterTraversalState &State = Context.Traversal;
    if (State.Action == EBBBTraversalAction::None)
    {
        return;
    }

    // 生命阶段改变即结束动作 不等待播放尾段再交还移动权
    if (Context.Life.Phase != EBBBCharacterLifePhase::Alive)
    {
        State.bEndRequested = true;
    }

    const bool bOwnPlayback = Context.Playback.LastActionId == State.ActionId;
    // 尚未形成播放贡献的动作可以直接释放 已启动的各端都必须等待根运动退出确认
    State.bAnimationReleased = bOwnPlayback
        ? Context.Playback.bRootMotionReleased
        : State.bEndRequested && !State.bPlaybackObserved;
    if (bOwnPlayback && Context.Playback.bPlaying)
    {
        State.PlaybackPosition = Context.Playback.Position;
        State.bPlaybackObserved |= Context.Playback.bPlaying;
    }

    // 必须同时得到动画退出与 CMC 释放结果 才能清空业务状态和校正目标
    if (State.bEndRequested && State.bAnimationReleased && !Context.Locomotion.bTraversalControlled)
    {
        State.Action = EBBBTraversalAction::None;
        State.bEndRequested = false;
        State.Obstacle.Reset();
        return;
    }

    if (Context.Execution.bIsMirror)
    {
        /** 镜像只执行结束结果的播放清理 不重新决定动作生命周期 */
        State.bPlaybackRequested = true;
        return;
    }

    if (Context.Montages.HasFullBodyRequest())
    {
        // 全身请求只中断已有动作 不再充当攀爬播放的启动确认
        State.bEndRequested |= State.bPlaybackRequested;
    }
    State.bPlaybackRequested |= bOwnPlayback && Context.Playback.MontageInstanceId != INDEX_NONE;

    const bool bEnded = State.bPlaybackObserved && bOwnPlayback && !Context.Playback.bPlaying;
    const double Elapsed = Context.World.WorldTimeSeconds - State.StartTime;
    const bool bFailed = !State.bPlaybackObserved && Elapsed > Context.TraversalConfig.StartTimeout;
    const bool bFault = bFailed || Elapsed > Context.TraversalConfig.MaxDuration || !State.Obstacle.IsValid();

    if (bFault)
    {
        /** 异常结束只发布退出请求 由动画晚更新暂停根运动并保留状态混合尾姿 */
        State.bEndRequested = true;
    }

    // 最后一个校正窗口完成后只在脚下已具备支撑时提前交还控制权
    const bool bWarpComplete = Context.Playback.bExitWindowReached;
    // 移动可以在主动作完成后接管 无输入保留整段姿势直到播放实际结束
    const bool bWantsMove = !Context.ControlState.MoveWorld.IsNearlyZero();
    const float InputExitTime = FMath::Max(Context.Playback.ContactWarpEndTime,
        Context.Playback.LastWarpEndTime - FMath::Max(Context.TraversalConfig.InputExitLeadTime, 0.0f));
    const bool bInputExitWindow = Context.Playback.ContactWarpEndTime > 0.0f
        && Context.Playback.Position >= InputExitTime;
    FFindFloorResult Floor;
    if (!State.bEndRequested && bOwnPlayback && bWantsMove && (bWarpComplete || bInputExitWindow))
    {
        const float FloorDistance = Context.TraversalConfig.Clearance + 5.0f;
        Context.Movement.ComputeFloorDist(Context.Character.GetActorLocation(), FloorDistance, FloorDistance,
            Floor, Context.Character.GetCapsuleComponent()->GetScaledCapsuleRadius());
        const FVector End = State.EndTarget.GetLocation();
        const FVector Feet = Context.Character.GetActorLocation()
            - FVector(0, 0, Context.Character.GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
        State.bEndRequested |= Floor.IsWalkableFloor() && !Floor.HitResult.bStartPenetrating
            && Floor.FloorDist <= FloorDistance
            && FVector::Dist2D(Feet, End) <= Context.Character.GetCapsuleComponent()->GetScaledCapsuleRadius()
            && FMath::Abs(Feet.Z - End.Z) <= FloorDistance;
    }
    State.bEndRequested |= bEnded;

    /** 翻越已离开背面碰撞区后由 CMC 完成落地 不再等待落地动画尾段 */
    if (!State.bEndRequested && bWantsMove && State.Action == EBBBTraversalAction::Vault && bOwnPlayback
        && Context.Playback.ContactWarpEndTime > 0.0f
        && Context.Playback.Position >= Context.Playback.ContactWarpEndTime)
    {
        const UCapsuleComponent *Capsule = Context.Character.GetCapsuleComponent();
        UWorld *World = Context.Character.GetWorld();
        const FVector Center = Context.Character.GetActorLocation();
        const FVector Feet = Center - FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight());
        const FVector End = State.EndTarget.GetLocation();
        const FVector Forward = State.EndTarget.GetRotation().GetForwardVector();
        const FVector Delta = Feet - End;
        const float Along = FVector::DotProduct(Delta, Forward);
        const float Across = FVector::DotProduct(Delta, FVector::CrossProduct(Forward, FVector::UpVector));
        const FCollisionShape Shape = FCollisionShape::MakeCapsule(
            Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight());
        FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBTraversalRelease), false, &Context.Character);
        FHitResult Hit;
        const FVector LandingCenter(Center.X, Center.Y, End.Z + Capsule->GetScaledCapsuleHalfHeight());
        if (World && Along >= -Context.TraversalConfig.Clearance && Feet.Z >= End.Z
            && FMath::Abs(Across) <= Capsule->GetScaledCapsuleRadius()
            && !World->OverlapBlockingTestByChannel(Center, FQuat::Identity, ECC_Pawn, Shape, Query)
            && !World->SweepSingleByChannel(Hit, Center, LandingCenter, FQuat::Identity, ECC_Pawn, Shape, Query))
        {
            State.bEndRequested = true;
        }
    }
}
