#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterAimImpulseProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAimImpulseProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.AimImpulseState;
    auto &Facts = Context.RuntimeData.Animation.AnimationFactState;
    const auto &Config = Context.AnimationConfig;
    const float DeltaSeconds = Context.WorldState.FrameDeltaSeconds;
    if (!ensureMsgf(
        FMath::IsFinite(Config.AimImpulseRecoverySpeed) && Config.AimImpulseRecoverySpeed > 0.0f
            && !Config.AimImpulseLimitDegrees.ContainsNaN()
            && Config.AimImpulseLimitDegrees.X > 0.0f && Config.AimImpulseLimitDegrees.Y > 0.0f,
        TEXT("角色瞄准冲击恢复速度或角度限制无效")))
    {
        State.PendingDegrees = FVector2D::ZeroVector;
        State.OffsetDegrees = FVector2D::ZeroVector;
        return;
    }

    if (!Context.RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveMainHandInstance)
    {
        State.PendingDegrees = FVector2D::ZeroVector;
        State.OffsetDegrees = FVector2D::ZeroVector;
        return;
    }

    State.OffsetDegrees *= FMath::Exp(-Config.AimImpulseRecoverySpeed * FMath::Max(DeltaSeconds, 0.0f));
    State.OffsetDegrees += State.PendingDegrees;
    State.PendingDegrees = FVector2D::ZeroVector;
    State.OffsetDegrees.X = FMath::Clamp(State.OffsetDegrees.X, -Config.AimImpulseLimitDegrees.X, Config.AimImpulseLimitDegrees.X);
    State.OffsetDegrees.Y = FMath::Clamp(State.OffsetDegrees.Y, -Config.AimImpulseLimitDegrees.Y, Config.AimImpulseLimitDegrees.Y);

    const FName HandBoneName(TEXT("hand_r"));
    if (Context.CharacterMesh.GetBoneIndex(HandBoneName) == INDEX_NONE)
    {
        return;
    }

    const FTransform MuzzleComponent = Facts.MuzzleTransformHandRSpace
        * Context.CharacterMesh.GetSocketTransform(HandBoneName, RTS_Component);
    const FVector Origin = MuzzleComponent.GetLocation();
    const FVector TargetDelta = Facts.AimTargetComponentSpace - Origin;
    const double Distance = TargetDelta.Size();
    if (!Facts.bHasSmoothedAimTarget || Distance <= UE_SMALL_NUMBER || TargetDelta.ContainsNaN())
    {
        return;
    }

    const FVector Direction = TargetDelta / Distance;
    const FVector Up = Context.CharacterMesh.GetComponentTransform().InverseTransformVectorNoScale(FVector::UpVector).GetSafeNormal();
    const FVector Right = FVector::CrossProduct(Up, Direction).GetSafeNormal();
    if (Right.IsNearlyZero())
    {
        return;
    }

    const FQuat Pitch(Right, FMath::DegreesToRadians(-State.OffsetDegrees.X));
    const FQuat Yaw(Up, FMath::DegreesToRadians(State.OffsetDegrees.Y));
    Facts.AimTargetComponentSpace = Origin + Yaw.RotateVector(Pitch.RotateVector(Direction)) * Distance;
}
