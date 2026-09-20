#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

UBBBAnimInstance *UBBBAnimInstance::GetBBBMainAnimInstanceThreadSafe() const
{
    // 优先取得骨骼网格上的主动画实例保证链接层读取统一状态
    UBBBAnimInstance *MainAnimInstance = Cast<UBBBAnimInstance>(Blueprint_GetMainAnimInstance());
    if (MainAnimInstance)
    {
        return MainAnimInstance;
    }

    return const_cast<UBBBAnimInstance *>(this);
}

//------------------------------------------------------------------------------

void UBBBAnimInstance::PublishAnimationFacts(
    const FBBBCharacterAnimationFacts &Facts)
{
    // 将角色运行事实复制到动画实例供动画图安全读取
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

UBBBEquipmentAnimInstance *UBBBAnimInstance::TryGetWeaponAnimInstance() const
{
    // 武器动画实例允许为空未装备时由调用方获得默认回退
    return GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance.Get();
}

FTransform UBBBAnimInstance::TryGetWeaponAimSourceLocalTransform() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->GetAimSourceLocalTransform();
    }

    return FTransform::Identity;
}

FVector UBBBAnimInstance::TryGetWeaponLeftHandTargetHandRSpace() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->GetLeftHandTargetHandRSpace();
    }

    return FVector::ZeroVector;
}

bool UBBBAnimInstance::TryHasWeaponLeftHandTarget() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->HasLeftHandTarget();
    }

    return false;
}

bool UBBBAnimInstance::TryGetWeaponReloading() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->IsReloading();
    }

    return false;
}

float UBBBAnimInstance::TryGetWeaponTimeSinceLastFireSeconds() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->GetCurrentWorldTimeSeconds() - Weapon->GetLastFireTimeSeconds();
    }

    return 1.0e+38f;
}

void UBBBAnimInstance::BindWeaponAnimInstance(UBBBEquipmentAnimInstance *InWeaponAnimInstance)
{
    GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance = InWeaponAnimInstance;
}
