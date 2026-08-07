#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionFactsProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionFactsProcessor::Update(
    const UCharacterMovementComponent &Movement,
    const FBBBAimRuntimeData &AimData,
    const FBBBFacingRuntimeData &FacingData,
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterAnimationState &AnimationState) const
{
    const AActor *Owner = Movement.GetOwner();
    if (!ensureMsgf(Owner, TEXT("[UBBBC]Locomotion facts update failed because movement owner is null")))
    {
        return;
    }

    const FVector Velocity = Movement.Velocity;
    const FVector LocalVelocity = Owner->GetActorTransform().InverseTransformVectorNoScale(Velocity);
    const float GroundSpeed = Velocity.Size2D();
    const FBBBAimRuntimeState &AimState = AimData.GetState();

    AnimationState.bIsAiming = AimState.bIsAiming;
    AnimationState.bIsMoving = GroundSpeed > KINDA_SMALL_NUMBER;
    AnimationState.bIsGrounded = Movement.IsMovingOnGround();
    AnimationState.bHasMainHandEquipment = EquipmentState.GetActiveMainHandInstance() != nullptr;
    AnimationState.bIsTurningInPlace = FacingData.IsBodyTurning();
    AnimationState.GroundSpeed = GroundSpeed;
    AnimationState.LocalForwardSpeed = LocalVelocity.X;
    AnimationState.LocalRightSpeed = LocalVelocity.Y;
    AnimationState.VerticalSpeed = Velocity.Z;
    AnimationState.AimYaw = AimState.AimYaw;
}
