#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Diagnostics/BBBLocomotionRuntimeProbe.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimSync.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAL/IConsoleManager.h"
#include "UObject/UnrealType.h"

#include <limits>

DEFINE_LOG_CATEGORY_STATIC(LogBBBLocomotionProbe, Log, All);

namespace
{
TAutoConsoleVariable<int32> CVarBBBLocomotionRuntimeProbe(
    TEXT("bbb.Animation.LocomotionProbe"),
    0,
    TEXT("启用 BBB 移动动画运行时探针。0=关闭，1=逐帧记录。"),
    ECVF_Default);

float ReadFloatProperty(const UObject &Object, const FName PropertyName)
{
    const FNumericProperty *Property = FindFProperty<FNumericProperty>(
        Object.GetClass(),
        PropertyName);
    if (!Property || !Property->IsFloatingPoint())
    {
        return std::numeric_limits<float>::quiet_NaN();
    }

    const void *ValueAddress = Property->ContainerPtrToValuePtr<void>(&Object);
    return static_cast<float>(Property->GetFloatingPointPropertyValue(ValueAddress));
}

bool ReadBoolProperty(
    const UObject &Object,
    const FName PropertyName,
    bool &bOutValue)
{
    const FBoolProperty *Property = FindFProperty<FBoolProperty>(
        Object.GetClass(),
        PropertyName);
    if (!Property)
    {
        return false;
    }

    bOutValue = Property->GetPropertyValue_InContainer(&Object);
    return true;
}

FVector ReadVectorProperty(const UObject &Object, const FName PropertyName)
{
    const FStructProperty *Property = FindFProperty<FStructProperty>(
        Object.GetClass(),
        PropertyName);
    if (!Property || Property->Struct != TBaseStructure<FVector>::Get())
    {
        return FVector(
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN());
    }

    return *Property->ContainerPtrToValuePtr<FVector>(&Object);
}

void LogSyncPlayers(
    const UAnimInstance &AnimInstance,
    const FAnimInstanceProxy &AnimationProxy,
    const TCHAR *InstanceRole)
{
    for (const TPair<FName, FAnimGroupInstance> &SyncGroupPair : AnimationProxy.GetSyncGroupMapRead())
    {
        const FAnimGroupInstance &SyncGroup = SyncGroupPair.Value;
        for (int32 PlayerIndex = 0; PlayerIndex < SyncGroup.ActivePlayers.Num(); ++PlayerIndex)
        {
            const FAnimTickRecord &Player = SyncGroup.ActivePlayers[PlayerIndex];
            const float PlaybackTime = Player.TimeAccumulator
                ? *Player.TimeAccumulator
                : Player.Montage.CurrentPosition;

            UE_LOG(
                LogBBBLocomotionProbe,
                Log,
                TEXT("BBB_ANIM_PLAYER Frame=%llu InstanceRole=%s Instance=%s Group=%s Role=%s Asset=%s Time=%.6f Rate=%.6f Weight=%.6f Looping=%d MarkerSync=%d GroupPosition=%.6f RootMotionWeight=%.6f"),
                GFrameCounter,
                InstanceRole,
                *AnimInstance.GetClass()->GetPathName(),
                *SyncGroupPair.Key.ToString(),
                PlayerIndex == SyncGroup.GroupLeaderIndex ? TEXT("Leader") : TEXT("Follower"),
                *GetNameSafe(Player.SourceAsset),
                PlaybackTime,
                Player.PlayRateMultiplier,
                Player.EffectiveBlendWeight,
                Player.bLooping,
                SyncGroup.bCanUseMarkerSync,
                SyncGroup.AnimLengthRatio,
                Player.GetRootMotionWeight());
        }
    }
}
}

void FBBBLocomotionRuntimeProbe::Reset()
{
    PreviousActorLocation = FVector::ZeroVector;
    PreviousPelvisLocation = FVector::ZeroVector;
    PreviousLeftFootLocation = FVector::ZeroVector;
    PreviousRightFootLocation = FVector::ZeroVector;
    PreviousSampleTime = 0.0;
    bHasPreviousSample = false;
}

//------------------------------------------------------------------------------

bool FBBBLocomotionRuntimeProbe::IsEnabled()
{
    return CVarBBBLocomotionRuntimeProbe.GetValueOnGameThread() != 0;
}

//------------------------------------------------------------------------------

void FBBBLocomotionRuntimeProbe::CaptureMainInstance(
    const UBBBAnimInstance &AnimInstance,
    const FAnimInstanceProxy &AnimationProxy)
{
    const ACharacter *Character = Cast<ACharacter>(AnimInstance.TryGetPawnOwner());
    const USkeletalMeshComponent *Mesh = AnimInstance.GetSkelMeshComponent();
    const UCharacterMovementComponent *Movement = Character
        ? Character->GetCharacterMovement()
        : nullptr;
    const UWorld *World = AnimInstance.GetWorld();

    if (!Character || !Mesh || !Movement || !World || !Character->IsLocallyControlled())
    {
        return;
    }

    const double SampleTime = World->GetTimeSeconds();
    const double DeltaSeconds = bHasPreviousSample
        ? SampleTime - PreviousSampleTime
        : 0.0;
    const FVector ActorLocation = Character->GetActorLocation();
    const FVector PelvisLocation = Mesh->GetBoneLocation(TEXT("pelvis"));
    const FVector LeftFootLocation = Mesh->GetBoneLocation(TEXT("foot_l"));
    const FVector RightFootLocation = Mesh->GetBoneLocation(TEXT("foot_r"));
    const FVector ActorDelta = bHasPreviousSample
        ? ActorLocation - PreviousActorLocation
        : FVector::ZeroVector;
    const FVector PelvisDelta = bHasPreviousSample
        ? PelvisLocation - PreviousPelvisLocation
        : FVector::ZeroVector;
    const FVector LeftFootDelta = bHasPreviousSample
        ? LeftFootLocation - PreviousLeftFootLocation
        : FVector::ZeroVector;
    const FVector RightFootDelta = bHasPreviousSample
        ? RightFootLocation - PreviousRightFootLocation
        : FVector::ZeroVector;
    const double SafeDeltaSeconds = DeltaSeconds > UE_SMALL_NUMBER
        ? DeltaSeconds
        : 1.0;

    bool bBlueprintHasVelocity = false;
    bool bBlueprintHasAcceleration = false;
    bool bUseFootPlacement = false;
    const bool bHasBlueprintVelocityProperty = ReadBoolProperty(
        AnimInstance,
        TEXT("hasVelocity_0"),
        bBlueprintHasVelocity);
    const bool bHasBlueprintAccelerationProperty = ReadBoolProperty(
        AnimInstance,
        TEXT("hasAcceleration_0"),
        bBlueprintHasAcceleration);
    const bool bHasUseFootPlacementProperty = ReadBoolProperty(
        AnimInstance,
        TEXT("useFootPlacement"),
        bUseFootPlacement);
    const FVector BlueprintLocalVelocity = ReadVectorProperty(
        AnimInstance,
        TEXT("localVelocity2D"));
    const FVector BlueprintLocalAcceleration = ReadVectorProperty(
        AnimInstance,
        TEXT("localAcceleration2D"));
    const float BlueprintDisplacementSpeed = ReadFloatProperty(
        AnimInstance,
        TEXT("displacementSpeed"));
    const float BlueprintDirectionAngle = ReadFloatProperty(
        AnimInstance,
        TEXT("localVelocityDirectionAngle"));
    const bool bBlueprintDriven = bBlueprintHasVelocity
        && bBlueprintHasAcceleration;
    const bool bBlueprintStopping = bBlueprintHasVelocity
        && !bBlueprintHasAcceleration;

    UE_LOG(
        LogBBBLocomotionProbe,
        Log,
        TEXT("BBB_LOCOMOTION_FRAME Frame=%llu Time=%.6f Character=%s Velocity=%s CurrentAcceleration=%s LastInput=%s ActualDisplacementSpeed=%.6f BlueprintDisplacementSpeed=%.6f LocalForward=%.6f LocalRight=%.6f DirectionAngle=%.6f MovementMode=%d Moving=%d Driven=%d Stopping=%d ActorYaw=%.6f ControlYaw=%.6f MaxAcceleration=%.6f BrakingDeceleration=%.6f GroundFriction=%.6f BrakingFriction=%.6f BrakingFrictionFactor=%.6f OrientToMovement=%d ControllerDesiredRotation=%d ControllerYaw=%d"),
        GFrameCounter,
        SampleTime,
        *Character->GetName(),
        *Movement->Velocity.ToCompactString(),
        *Movement->GetCurrentAcceleration().ToCompactString(),
        *Movement->GetLastInputVector().ToCompactString(),
        ActorDelta.Size2D() / SafeDeltaSeconds,
        BlueprintDisplacementSpeed,
        BlueprintLocalVelocity.X,
        BlueprintLocalVelocity.Y,
        BlueprintDirectionAngle,
        static_cast<uint8>(AnimInstance.SourceMovementMode),
        bBlueprintHasVelocity,
        bBlueprintDriven,
        bBlueprintStopping,
        Character->GetActorRotation().Yaw,
        Character->GetControlRotation().Yaw,
        Movement->MaxAcceleration,
        Movement->BrakingDecelerationWalking,
        Movement->GroundFriction,
        Movement->BrakingFriction,
        Movement->BrakingFrictionFactor,
        Movement->bOrientRotationToMovement,
        Movement->bUseControllerDesiredRotation,
        Character->bUseControllerRotationYaw);

    UE_LOG(
        LogBBBLocomotionProbe,
        Log,
        TEXT("BBB_CHARACTER_ANIMATION_FACTS Frame=%llu Gait=%d MovementMode=%d Walking=%d Running=%d Sprinting=%d HasEquipment=%d Aiming=%d AimIntentAlpha=%.6f Reloading=%d TimeSinceLastFire=%.6f ValidAimSource=%d ValidAimTarget=%d AimIKAlpha=%.6f"),
        GFrameCounter,
        static_cast<uint8>(AnimInstance.GetGait()),
        static_cast<uint8>(AnimInstance.SourceMovementMode),
        AnimInstance.IsWalking(),
        AnimInstance.IsRunning(),
        AnimInstance.IsSprinting(),
        AnimInstance.HasMainHandEquipment(),
        AnimInstance.IsAiming(),
        AnimInstance.GetAimIntentAlpha(),
        AnimInstance.IsReloading(),
        AnimInstance.GetTimeSinceLastFire(),
        AnimInstance.HasValidAimSource(),
        AnimInstance.HasValidAimTarget(),
        AnimInstance.GetAimIKAlpha());

    UE_LOG(
        LogBBBLocomotionProbe,
        Log,
        TEXT("BBB_ANIM_GRAPH_VALUES Frame=%llu LocalVelocity2D=%s LocalAcceleration2D=%s BlueprintDisplacementSpeed=%.6f BlueprintDirectionAngle=%.6f BlueprintHasVelocity=%d BlueprintHasVelocityProperty=%d BlueprintHasAcceleration=%d BlueprintHasAccelerationProperty=%d UseFootPlacement=%d UseFootPlacementProperty=%d DisableLegIK=%.6f DisableFootLocking=%.6f Distance=%.6f Speed=%.6f"),
        GFrameCounter,
        *BlueprintLocalVelocity.ToCompactString(),
        *BlueprintLocalAcceleration.ToCompactString(),
        BlueprintDisplacementSpeed,
        BlueprintDirectionAngle,
        bBlueprintHasVelocity,
        bHasBlueprintVelocityProperty,
        bBlueprintHasAcceleration,
        bHasBlueprintAccelerationProperty,
        bUseFootPlacement,
        bHasUseFootPlacementProperty,
        AnimInstance.GetCurveValue(TEXT("DisableLegIK")),
        AnimInstance.GetCurveValue(TEXT("DisableFootLocking")),
        AnimInstance.GetCurveValue(TEXT("Distance")),
        AnimInstance.GetCurveValue(TEXT("Speed")));

    UE_LOG(
        LogBBBLocomotionProbe,
        Log,
        TEXT("BBB_FINAL_POSE Frame=%llu Pelvis=%s LeftFoot=%s RightFoot=%s ActorDelta=%s PelvisWorldSpeed=%.6f LeftFootWorldSpeed=%.6f RightFootWorldSpeed=%.6f PelvisMeshRelativeSpeed=%.6f LeftFootMeshRelativeSpeed=%.6f RightFootMeshRelativeSpeed=%.6f LeftFootHeightToPelvis=%.6f RightFootHeightToPelvis=%.6f"),
        GFrameCounter,
        *PelvisLocation.ToCompactString(),
        *LeftFootLocation.ToCompactString(),
        *RightFootLocation.ToCompactString(),
        *ActorDelta.ToCompactString(),
        PelvisDelta.Size2D() / SafeDeltaSeconds,
        LeftFootDelta.Size2D() / SafeDeltaSeconds,
        RightFootDelta.Size2D() / SafeDeltaSeconds,
        (PelvisDelta - ActorDelta).Size2D() / SafeDeltaSeconds,
        (LeftFootDelta - ActorDelta).Size2D() / SafeDeltaSeconds,
        (RightFootDelta - ActorDelta).Size2D() / SafeDeltaSeconds,
        LeftFootLocation.Z - PelvisLocation.Z,
        RightFootLocation.Z - PelvisLocation.Z);

    LogSyncPlayers(AnimInstance, AnimationProxy, TEXT("Main"));

    PreviousActorLocation = ActorLocation;
    PreviousPelvisLocation = PelvisLocation;
    PreviousLeftFootLocation = LeftFootLocation;
    PreviousRightFootLocation = RightFootLocation;
    PreviousSampleTime = SampleTime;
    bHasPreviousSample = true;
}

//------------------------------------------------------------------------------

void FBBBLocomotionRuntimeProbe::CaptureLinkedInstance(
    const UAnimInstance &AnimInstance,
    const FAnimInstanceProxy &AnimationProxy) const
{
    UE_LOG(
        LogBBBLocomotionProbe,
        Log,
        TEXT("BBB_LINKED_LAYER Frame=%llu Class=%s DisplacementSpeed=%.6f StrideCycleAlpha=%.6f StrideStartAlpha=%.6f StridePivotAlpha=%.6f OrientationAngle=%.6f DisableLegIK=%.6f DisableFootLocking=%.6f Distance=%.6f Speed=%.6f RootMotion=%d"),
        GFrameCounter,
        *AnimInstance.GetClass()->GetPathName(),
        ReadFloatProperty(AnimInstance, TEXT("DisplacementSpeed")),
        ReadFloatProperty(AnimInstance, TEXT("StrideWarpingCycleAlpha")),
        ReadFloatProperty(AnimInstance, TEXT("StrideWarpingStartAlpha")),
        ReadFloatProperty(AnimInstance, TEXT("StrideWarpingPivotAlpha")),
        ReadFloatProperty(AnimInstance, TEXT("OrientationAngle")),
        AnimInstance.GetCurveValue(TEXT("DisableLegIK")),
        AnimInstance.GetCurveValue(TEXT("DisableFootLocking")),
        AnimInstance.GetCurveValue(TEXT("Distance")),
        AnimInstance.GetCurveValue(TEXT("Speed")),
        AnimInstance.ShouldExtractRootMotion());

    LogSyncPlayers(AnimInstance, AnimationProxy, TEXT("Linked"));
}
