#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"

#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    LocomotionRuntimeProbe.Reset();
}

//------------------------------------------------------------------------------

void UBBBAnimInstance::NativePostEvaluateAnimation()
{
    Super::NativePostEvaluateAnimation();

    if (!FBBBLocomotionRuntimeProbe::IsEnabled())
    {
        return;
    }

    const FAnimInstanceProxy &MainProxy = GetProxyOnGameThread<FAnimInstanceProxy>();
    LocomotionRuntimeProbe.CaptureMainInstance(*this, MainProxy);

    const USkeletalMeshComponent *Mesh = GetSkelMeshComponent();
    if (!Mesh)
    {
        return;
    }

    for (UAnimInstance *LinkedInstance : Mesh->GetLinkedAnimInstances())
    {
        if (!LinkedInstance)
        {
            continue;
        }

        const FAnimInstanceProxy *LinkedProxy = GetProxyOnGameThreadStatic<FAnimInstanceProxy>(
            LinkedInstance);
        if (!LinkedProxy)
        {
            continue;
        }

        LocomotionRuntimeProbe.CaptureLinkedInstance(
            *LinkedInstance,
            *LinkedProxy);
    }
}

//------------------------------------------------------------------------------

void UBBBAnimInstance::PublishAnimationFacts(
    const FBBBCharacterAnimationFacts &Facts)
{
    AnimationFacts = Facts;

    SourceActorLocation = Facts.ActorLocation;
    SourceActorRotation = Facts.ActorRotation;
    SourceVelocity = Facts.Velocity;
    SourceLastUpdateVelocity = Facts.LastUpdateVelocity;
    SourceAcceleration = Facts.Acceleration;
    SourceMovementMode = Facts.MovementMode;

    SourceGroundFriction = Facts.GroundFriction;
    SourceBrakingFriction = Facts.BrakingFriction;
    SourceBrakingFrictionFactor = Facts.BrakingFrictionFactor;
    SourceBrakingDecelerationWalking = Facts.BrakingDecelerationWalking;
    bSourceUseSeparateBrakingFriction = Facts.bUseSeparateBrakingFriction;
    SourceGravityZ = Facts.GravityZ;

    bSourceMovingOnGround = Facts.bIsMovingOnGround;
    bSourceCrouching = Facts.bIsCrouching;
    GroundDistance = Facts.GroundDistance;
}

//------------------------------------------------------------------------------

void UBBBAnimInstance::PublishEquipmentAction(
    EBBBCharacterActionType ActionType,
    int32 Sequence,
    float Duration,
    UAnimMontage &Montage,
    float PlayRate)
{
    EquipmentActionType = ActionType;
    EquipmentActionSequence = Sequence;
    EquipmentActionDuration = Duration;
    EquipmentActionMontage = &Montage;
    EquipmentActionPlayRate = PlayRate;

    ExecuteEquipmentActionMontage(
        EquipmentActionType,
        EquipmentActionMontage,
        EquipmentActionPlayRate);
}
