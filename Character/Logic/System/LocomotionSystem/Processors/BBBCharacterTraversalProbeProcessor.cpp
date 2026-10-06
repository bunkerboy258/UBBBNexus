#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterTraversalProbeProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBTraversalConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterTraversalProbeProcessor::Update(FBBBCharacterLocomotionUpdateContext &Context) const
{
    const FBBBTraversalConfig &Config = Context.TraversalConfig;
    if (Context.Execution.bIsMirror || !Config.bEnabled || !Context.ControlState.bJump
        || Context.ControlState.bCrouch || !Context.Movement.IsMovingOnGround()
        || Context.Traversal.Action != EBBBTraversalAction::None || Context.Character.bIsCrouched
        || Context.AnimationFacts.bFullBodyPlaying || Context.Montages.HasFullBodyRequest())
    {
        return;
    }
    UWorld *World = Context.Character.GetWorld();
    const UCapsuleComponent *Capsule = Context.Character.GetCapsuleComponent();
    if (!World || !Capsule)
    {
        return;
    }
    const float Radius = Capsule->GetScaledCapsuleRadius();
    const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
    const FVector Feet = Context.Character.GetActorLocation() - FVector(0, 0, HalfHeight);
    const FVector Forward = FRotator(0, Context.ControlState.FacingWorld.Yaw, 0).Vector();
    FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBTraversal), false, &Context.Character);
    FHitResult Front;
    const FVector ProbeStart = Feet + FVector(0, 0, Config.MinHeight);
    if (!World->SweepSingleByChannel(Front, ProbeStart, ProbeStart + Forward * Config.ProbeDistance,
        FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Config.ProbeRadius), Query)
        || Front.bStartPenetrating || FVector::DotProduct(Front.ImpactNormal, -Forward) < 0.5f)
    {
        return;
    }
    UPrimitiveComponent *Obstacle = Front.GetComponent();
    if (!Obstacle || Obstacle->Mobility != EComponentMobility::Static)
    {
        return;
    }
    /** 顶部检测使用角色真实地面高度 避免把胶囊中心当成根骨 */
    const FVector Edge = Front.ImpactPoint + Forward * (Config.ProbeRadius + Config.Clearance);
    FHitResult Top;
    if (!World->LineTraceSingleByChannel(Top,
        FVector(Edge.X, Edge.Y, Feet.Z + Config.MaxHeight + Config.Clearance),
        FVector(Edge.X, Edge.Y, Feet.Z + Config.MinHeight - Config.Clearance), ECC_Pawn, Query)
        || Top.GetComponent() != Obstacle || !Context.Movement.IsWalkable(Top))
    {
        return;
    }
    const float Height = Top.ImpactPoint.Z - Feet.Z;
    if (Height < Config.MinHeight || Height > Config.MaxHeight)
    {
        return;
    }
    FVector End = Top.ImpactPoint + Forward * (Radius + Config.Clearance);
    EBBBTraversalAction Action = Height <= Config.LowMaxHeight
        ? EBBBTraversalAction::ClimbLow : EBBBTraversalAction::ClimbHigh;
    /** 只对低障碍寻找背面 深平台保持攀爬语义 */
    if (Height <= Config.LowMaxHeight)
    {
        const float Step = FMath::Max(Config.TopSampleSpacing, 1.0f);
        const float Search = FMath::Min(Config.TopSearchDistance, Config.VaultMaxDepth);
        for (float Distance = Step; Distance <= Search; Distance += Step)
        {
            const FVector Sample = Top.ImpactPoint + Forward * Distance;
            FHitResult Back;
            const bool bTop = World->LineTraceSingleByChannel(Back,
                Sample + FVector(0, 0, Config.Clearance + 5),
                Sample - FVector(0, 0, Config.Clearance + 10), ECC_Pawn, Query);
            if (bTop && Context.Movement.IsWalkable(Back))
            {
                continue;
            }
            const FVector Landing = Sample + Forward * (Radius + Config.Clearance);
            FHitResult Ground;
            if (World->LineTraceSingleByChannel(Ground,
                FVector(Landing.X, Landing.Y, Top.ImpactPoint.Z + Config.Clearance),
                FVector(Landing.X, Landing.Y, Feet.Z - Context.Movement.MaxStepHeight), ECC_Pawn, Query)
                && Context.Movement.IsWalkable(Ground))
            {
                End = Ground.ImpactPoint;
                Action = EBBBTraversalAction::Vault;
            }
            break;
        }
    }
    /** 脚底目标对应官方 Character Adapter 的 VisualRootLocation */
    End.Z += Config.Clearance;
    const FVector EndCenter = End + FVector(0, 0, HalfHeight);
    const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
    if (World->OverlapBlockingTestByChannel(EndCenter, FQuat::Identity, ECC_Pawn, Shape, Query))
    {
        return;
    }
    /** 检查先上升再越过边缘的胶囊通道 不关闭障碍碰撞 */
    const FVector Raised = FVector(Feet.X, Feet.Y, Top.ImpactPoint.Z + Config.Clearance + HalfHeight);
    const FVector Across = FVector(EndCenter.X, EndCenter.Y, Raised.Z);
    FHitResult Clearance;
    if (World->SweepSingleByChannel(Clearance, Context.Character.GetActorLocation(), Raised,
            FQuat::Identity, ECC_Pawn, Shape, Query)
        || World->SweepSingleByChannel(Clearance, Raised, Across,
            FQuat::Identity, ECC_Pawn, Shape, Query)
        || World->SweepSingleByChannel(Clearance, Across, EndCenter,
            FQuat::Identity, ECC_Pawn, Shape, Query))
    {
        return;
    }
    FBBBCharacterTraversalState &State = Context.Traversal;
    State.Action = Action;
    ++State.ActionId;
    if (State.ActionId == 0)
    {
        ++State.ActionId;
    }
    State.ContactTarget = FTransform(Forward.Rotation(), Top.ImpactPoint);
    State.EndTarget = FTransform(Forward.Rotation(), End);
    State.Obstacle = Obstacle;
    State.StartTime = Context.World.WorldTimeSeconds;
    State.bPlaybackRequested = false;
    State.bPlaybackObserved = false;
    State.bEndRequested = false;
    UE_LOG(LogTemp, Display, TEXT("BBBTraversal start id=%u action=%d height=%.1f end=%s"),
        State.ActionId, int32(Action), Height, *End.ToString());
}
