#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterAnimationFactProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Chooser.h"

namespace
{
constexpr float GroundTraceDistance = 100000.0f;
constexpr float ObstacleTraceDistance = 3.0f;
constexpr float ObstacleRadiusInset = 0.25f;
constexpr float ObstacleHeightInset = 1.0f;
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
    UClass* LayerClass = RuntimeData.Animation.ReadAnimationLayerState().LinkedAnimationLayerClass.Get();
    const UBBBAnimInstance* LayerDefaults = LayerClass ? LayerClass->GetDefaultObject<UBBBAnimInstance>() : nullptr;
    FactState.LocomotionDatabaseChooser = LayerDefaults ? LayerDefaults->LocomotionDatabaseChooser.Get() : nullptr;
    FactState.LifePhase = RuntimeData.Life.ReadLifeState().Phase;
    const UBBBItemDefinition *ItemDefinition = Character.GetHeldItemDefinition();
    FactState.ItemType = ItemDefinition ? ItemDefinition->ItemType : EBBBItemType::Misc;
    const ABBBEquipment *Equipment = Character.GetHeldEquipment();
    const UBBBEquipmentDefinition *Definition = IsValid(Equipment) ? Equipment->GetDefinition() : nullptr;
    FactState.EquipmentType = IsValid(Definition) ? Definition->EquipmentType : EBBBEquipmentType::None;
    FactState.CharacterGripPose = IsValid(Definition) ? Definition->CharacterGripPose.Get() : nullptr;
    FactState.CharacterFireGripPose = IsValid(Definition) ? Definition->CharacterFireGripPose.Get() : nullptr;
    FactState.bFire = RuntimeData.Parse.ReadControlState().bFire;
    FactState.RightHandGripRotationOffset = IsValid(Definition) ? Definition->RightHandGripRotationOffset : FRotator::ZeroRotator;
    if (!ensureMsgf(!FactState.RightHandGripRotationOffset.ContainsNaN(), TEXT("[BBB][AnimationFacts] 装备右腕握持旋转必须有限")))
    {
        FactState.RightHandGripRotationOffset = FRotator::ZeroRotator;
    }
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

    FactState.bFullBodyPlaying = Context.AnimationInstance.FullBodyMontageContribution
        && Context.AnimationInstance.Montage_IsPlaying(Context.AnimationInstance.FullBodyMontageContribution);
    FactState.bIsAiming = AimState.bIsAiming;
    FactState.AimTargetComponentSpace = RawAimTargetComponentSpace;

    FactState.ActorLocation = Character.GetActorLocation();
    FactState.ActorRotation = Character.GetActorRotation();
    FactState.MeshWorldTransform = CharacterMesh->GetComponentTransform();
    FactState.Velocity = Movement->Velocity;
    FactState.LastUpdateVelocity = Movement->GetLastUpdateVelocity();
    // 镜像 CMC 不保证保留控制者加速度 表现只读取已经还原的原始移动事实
    FactState.Acceleration = Context.RuntimeData.External.ReadNetworkIdentityState().bIsMirror
        ? RuntimeData.Locomotion.ReadLocomotionState().RestoredAcceleration
        : Movement->GetCurrentAcceleration();
    FactState.MovementInput = Context.RuntimeData.External.ReadNetworkIdentityState().bIsMirror
        ? RuntimeData.Locomotion.ReadLocomotionState().RestoredMovementInput
        : RuntimeData.Parse.ReadControlState().MoveWorld;
    FactState.BlockingSurfaceNormal = FVector::ZeroVector;
    FactState.SecondaryBlockingSurfaceNormal = FVector::ZeroVector;

    const FVector InputDirection = FactState.MovementInput.GetSafeNormal2D();
    if (Movement->IsMovingOnGround() && !InputDirection.IsNearlyZero())
    {
        FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BBBAnimationBlockingSurface), false, &Character);
        FCollisionResponseParams ResponseParams;
        Movement->InitCollisionParams(QueryParams, ResponseParams);
        QueryParams.AddIgnoredActors(Capsule->GetMoveIgnoreActors());
        for (const UPrimitiveComponent *Ignored : Capsule->GetMoveIgnoreComponents())
        {
            QueryParams.AddIgnoredComponent(Ignored);
        }

        const FVector Start = Capsule->GetComponentLocation();
        const float Radius = FMath::Max(Capsule->GetScaledCapsuleRadius() - ObstacleRadiusInset, 1.0f);
        const float HalfHeight = FMath::Max(Capsule->GetScaledCapsuleHalfHeight() - ObstacleHeightInset, Radius);
        const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
        const auto ReadBlockingNormal = [&](const FVector &Direction)
        {
            FHitResult Hit;
            if (!World->SweepSingleByChannel(Hit, Start, Start + Direction * ObstacleTraceDistance,
                Capsule->GetComponentQuat(), Capsule->GetCollisionObjectType(), Shape, QueryParams, ResponseParams)
                || !Hit.bBlockingHit || Hit.bStartPenetrating
                || Hit.ImpactNormal.Z >= Movement->GetWalkableFloorZ()
                || Hit.ImpactPoint.Z <= Start.Z - Capsule->GetScaledCapsuleHalfHeight() + Movement->MaxStepHeight)
            {
                return FVector::ZeroVector;
            }

            const FVector Normal = Hit.ImpactNormal.GetSafeNormal2D();
            if (FVector::DotProduct(Direction, Normal) >= -0.01f)
            {
                return FVector::ZeroVector;
            }

            return Normal;
        };

        FactState.BlockingSurfaceNormal = ReadBlockingNormal(InputDirection);
        if (!FactState.BlockingSurfaceNormal.IsNearlyZero())
        {
            const FVector Tangent = FVector::VectorPlaneProject(InputDirection, FactState.BlockingSurfaceNormal).GetSafeNormal2D();
            if (!Tangent.IsNearlyZero())
            {
                FactState.SecondaryBlockingSurfaceNormal = ReadBlockingNormal(Tangent);
            }
        }
    }

    FactState.MaxAcceleration = Movement->GetMaxAcceleration();
    FactState.FacingWorld = Context.RuntimeData.External.ReadNetworkIdentityState().bIsMirror
        ? Character.GetActorRotation()
        : RuntimeData.Parse.ReadControlState().FacingWorld;
    FactState.ControlRotation = Character.GetControlRotation();
    FactState.bIsRunning = RuntimeData.Locomotion.ReadLocomotionState().bRun;
    FactState.MovementMode = Movement->MovementMode;
    FactState.GroundFriction = Movement->GroundFriction;
    FactState.BrakingFriction = Movement->BrakingFriction;
    FactState.BrakingFrictionFactor = Movement->BrakingFrictionFactor;
    FactState.BrakingDecelerationWalking = Movement->BrakingDecelerationWalking;
    FactState.MaxSpeed = Movement->GetMaxSpeed();
    FactState.BrakingSubStepTime = Movement->BrakingSubStepTime;
    FactState.GravityZ = Movement->GetGravityZ();
    FactState.GroundDistance = GroundDistance;
    FactState.bUseSeparateBrakingFriction = Movement->bUseSeparateBrakingFriction;
    FactState.bIsMovingOnGround = Movement->IsMovingOnGround();
    FactState.bIsCrouching = Movement->IsCrouching();
}
