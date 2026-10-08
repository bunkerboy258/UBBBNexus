#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterAnimationFactProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
constexpr float GroundTraceDistance = 100000.0f;
}

void FBBBCharacterAnimationFactProcessor::Update(
    FBBBCharacterAnimationUpdateContext &Context) const
{
    ABBBCharacter &Character = Context.Character;
    FBBBCharacterRuntimeData &RuntimeData = Context.RuntimeData;
    FBBBCharacterAnimationFactState &FactState = Context.AnimationFactState;

    // 采集动画事实前确认角色组件和世界对象有效
    UCharacterMovementComponent *Movement = Character.GetCharacterMovement();
    USkeletalMeshComponent *CharacterMesh = Character.GetMesh();
    UWorld *World = Character.GetWorld();
    UCapsuleComponent *Capsule = Character.GetCapsuleComponent();
    if (!Movement || !CharacterMesh || !World || !Capsule)
    {
        return;
    }

    const FBBBAimState &AimState = RuntimeData.Aim.ReadAimState();
    FactState.LifePhase = RuntimeData.Life.ReadLifeState().Phase;
    const auto &LifeAnimation = RuntimeData.Animation.ReadLifeAnimationState();
    FactState.DownedEntryElapsed = FactState.LifePhase == EBBBCharacterLifePhase::Downed
        && LifeAnimation.DownedEntryStartTime >= 0.0
        ? static_cast<float>(FMath::Max(Context.WorldState.WorldTimeSeconds - LifeAnimation.DownedEntryStartTime, 0.0))
        : -1.0f;
    const ABBBEquipment *Equipment = Character.GetActiveEquipment();
    const UBBBEquipmentDefinition *Definition = IsValid(Equipment)
        && RuntimeData.Equipment.ReadEquipmentUseState().bUsable ? Equipment->GetDefinition() : nullptr;
    FactState.EquipmentType = IsValid(Definition) ? Definition->EquipmentType : EBBBEquipmentType::None;
    const FBBBAimAnimationConfig &AimConfig = Character.GetCharacterConfig().AimAnimation;

    // 优先使用配置骨骼作为瞄准起点否则使用角色网格位置
    FVector AimOrigin = CharacterMesh->GetComponentLocation() + FVector(0.0f, 0.0f, 50.0f);
    if (!AimConfig.AimIKOriginBoneName.IsNone()
        && CharacterMesh->DoesSocketExist(AimConfig.AimIKOriginBoneName))
    {
        AimOrigin = CharacterMesh->GetSocketLocation(AimConfig.AimIKOriginBoneName);
    }

    const FVector AimTargetWorld = AimState.AimTargetWorld;
    const bool bCanUseAimTarget = !AimTargetWorld.ContainsNaN()
        && !AimOrigin.ContainsNaN()
        && !(AimTargetWorld - AimOrigin).IsNearlyZero();
    const FVector RawAimTargetComponentSpace = bCanUseAimTarget
        ? CharacterMesh->GetComponentTransform().InverseTransformPosition(AimTargetWorld)
        : FVector::ZeroVector;

    float GroundDistance = 0.0f;
    if (!Movement->IsMovingOnGround())
    {
        // 空中状态向下追踪地面距离供动画判断下落高度
        const FVector TraceStart = Character.GetActorLocation();
        const FVector TraceEnd = TraceStart - FVector(
            0.0f,
            0.0f,
            GroundTraceDistance + Capsule->GetScaledCapsuleHalfHeight());
        FCollisionQueryParams QueryParams(
            SCENE_QUERY_STAT(BBBAnimationGroundDistance),
            false,
            &Character);
        FHitResult Hit;

        GroundDistance = -1.0f;
        if (World->LineTraceSingleByChannel(
            Hit,
            TraceStart,
            TraceEnd,
            ECC_Visibility,
            QueryParams))
        {
            GroundDistance = FMath::Max(
                Hit.Distance - Capsule->GetScaledCapsuleHalfHeight(),
                0.0f);
        }
    }

    const FBBBCharacterTraversalState &Traversal = RuntimeData.Traversal.ReadTraversalState();
    FactState.bTraversing = Traversal.Action != EBBBTraversalAction::None && !Traversal.bAnimationReleased;
    FactState.bFullBodyPlaying = Context.AnimationInstance.FullBodyMontageContribution
        && Context.AnimationInstance.Montage_IsPlaying(Context.AnimationInstance.FullBodyMontageContribution);
    FactState.bIsAiming = AimState.bIsAiming;
    FactState.AimTargetComponentSpace = RawAimTargetComponentSpace;

    FactState.ActorLocation = Character.GetActorLocation();
    FactState.ActorRotation = Character.GetActorRotation();
    FactState.Velocity = Movement->Velocity;
    FactState.LastUpdateVelocity = Movement->GetLastUpdateVelocity();
    FactState.Acceleration = Movement->GetCurrentAcceleration();
    FactState.bIsRunning = RuntimeData.Locomotion.ReadLocomotionState().bRun;
    FactState.MovementMode = Movement->MovementMode;
    FactState.GroundFriction = Movement->GroundFriction;
    FactState.BrakingFriction = Movement->BrakingFriction;
    FactState.BrakingFrictionFactor = Movement->BrakingFrictionFactor;
    FactState.BrakingDecelerationWalking = Movement->BrakingDecelerationWalking;
    FactState.GravityZ = Movement->GetGravityZ();
    FactState.GroundDistance = GroundDistance;
    FactState.bUseSeparateBrakingFriction = Movement->bUseSeparateBrakingFriction;
    FactState.bIsMovingOnGround = Movement->IsMovingOnGround();
    FactState.bIsCrouching = Movement->IsCrouching();
}
