#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBLinkedAnimInstance.h"

#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

UBBBAnimInstance *UBBBLinkedAnimInstance::GetBBBMainAnimInstanceThreadSafe() const
{
    return Cast<UBBBAnimInstance>(Blueprint_GetMainAnimInstance());
}

//------------------------------------------------------------------------------

EBBBCharacterGait UBBBLinkedAnimInstance::GetLinkedGait() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return EBBBCharacterGait::Run;
    }

    return MainAnimInstance->GetGait();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::IsLinkedWalking() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->IsWalking();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::IsLinkedRunning() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->IsRunning();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::IsLinkedSprinting() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->IsSprinting();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::IsLinkedAiming() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->IsAiming();
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedAimIntentAlpha() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->GetAimIntentAlpha();
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedAimIKAlpha() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->GetAimIKAlpha();
}

//------------------------------------------------------------------------------

FVector UBBBLinkedAnimInstance::GetLinkedAimTargetComponentSpace() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FVector::ZeroVector;
    }

    return MainAnimInstance->GetAimTargetComponentSpace();
}

//------------------------------------------------------------------------------

FTransform UBBBLinkedAnimInstance::GetLinkedAimSourceLocalTransform() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FTransform::Identity;
    }

    return MainAnimInstance->GetAimSourceLocalTransform();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::HasLinkedValidAimTarget() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->HasValidAimTarget();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::HasLinkedValidAimSource() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->HasValidAimSource();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::HasLinkedMainHandEquipment() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->HasMainHandEquipment();
}

//------------------------------------------------------------------------------

UBBBEquipmentInstance *UBBBLinkedAnimInstance::GetLinkedMainHandEquipmentInstance() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return nullptr;
    }

    return MainAnimInstance->GetMainHandEquipmentInstance();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::IsLinkedReloading() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->IsReloading();
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedTimeSinceLastFire() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return BIG_NUMBER;
    }

    return MainAnimInstance->GetTimeSinceLastFire();
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::ShouldLinkedRaiseWeaponAfterFiring(const float Duration) const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->ShouldRaiseWeaponAfterFiring(Duration);
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::HasLinkedLeftHandIKOffsetRightHand() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    UBBBEquipmentInstance *EquipmentInstance = MainAnimInstance->GetMainHandEquipmentInstance();
    if (!EquipmentInstance)
    {
        return false;
    }

    return EquipmentInstance->HasValidLeftHandIKOffset();
}

//------------------------------------------------------------------------------

FVector UBBBLinkedAnimInstance::GetLinkedLeftHandIKOffsetRightHand() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FVector::ZeroVector;
    }

    UBBBEquipmentInstance *EquipmentInstance = MainAnimInstance->GetMainHandEquipmentInstance();
    if (!EquipmentInstance)
    {
        return FVector::ZeroVector;
    }

    return EquipmentInstance->GetLeftHandIKOffsetRightHand();
}

//------------------------------------------------------------------------------

FVector UBBBLinkedAnimInstance::GetLinkedSourceActorLocation() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FVector::ZeroVector;
    }

    return MainAnimInstance->SourceActorLocation;
}

//------------------------------------------------------------------------------

FRotator UBBBLinkedAnimInstance::GetLinkedSourceActorRotation() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FRotator::ZeroRotator;
    }

    return MainAnimInstance->SourceActorRotation;
}

//------------------------------------------------------------------------------

FVector UBBBLinkedAnimInstance::GetLinkedSourceVelocity() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FVector::ZeroVector;
    }

    return MainAnimInstance->SourceVelocity;
}

//------------------------------------------------------------------------------

FVector UBBBLinkedAnimInstance::GetLinkedSourceLastUpdateVelocity() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FVector::ZeroVector;
    }

    return MainAnimInstance->SourceLastUpdateVelocity;
}

//------------------------------------------------------------------------------

FVector UBBBLinkedAnimInstance::GetLinkedSourceAcceleration() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return FVector::ZeroVector;
    }

    return MainAnimInstance->SourceAcceleration;
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedSourceGroundFriction() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->SourceGroundFriction;
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedSourceBrakingFriction() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->SourceBrakingFriction;
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedSourceBrakingFrictionFactor() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->SourceBrakingFrictionFactor;
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedSourceBrakingDecelerationWalking() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->SourceBrakingDecelerationWalking;
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::HasLinkedSeparateBrakingFriction() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->bSourceUseSeparateBrakingFriction;
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedSourceGravityZ() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->SourceGravityZ;
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::IsLinkedMovingOnGround() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->bSourceMovingOnGround;
}

//------------------------------------------------------------------------------

bool UBBBLinkedAnimInstance::IsLinkedCrouching() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return false;
    }

    return MainAnimInstance->bSourceCrouching;
}

//------------------------------------------------------------------------------

float UBBBLinkedAnimInstance::GetLinkedGroundDistance() const
{
    const UBBBAnimInstance *MainAnimInstance = GetBBBMainAnimInstanceThreadSafe();
    if (!MainAnimInstance)
    {
        return 0.0f;
    }

    return MainAnimInstance->GroundDistance;
}
