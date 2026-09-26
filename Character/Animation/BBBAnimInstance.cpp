#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "Animation/AnimMontage.h"
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
    const FBBBCharacterAnimationFactState &FactState)
{
    // 将角色运行事实复制到动画实例供动画图安全读取
    SourceActorLocation = FactState.ActorLocation;
    SourceActorRotation = FactState.ActorRotation;
    SourceVelocity = FactState.Velocity;
    SourceLastUpdateVelocity = FactState.LastUpdateVelocity;
    SourceAcceleration = FactState.Acceleration;
    SourceMovementMode = FactState.MovementMode;
    bSourceRunning = FactState.bIsRunning;

    SourceGroundFriction = FactState.GroundFriction;
    SourceBrakingFriction = FactState.BrakingFriction;
    SourceBrakingFrictionFactor = FactState.BrakingFrictionFactor;
    SourceBrakingDecelerationWalking = FactState.BrakingDecelerationWalking;
    bSourceUseSeparateBrakingFriction = FactState.bUseSeparateBrakingFriction;
    SourceGravityZ = FactState.GravityZ;

    bSourceMovingOnGround = FactState.bIsMovingOnGround;
    bSourceCrouching = FactState.bIsCrouching;
    GroundDistance = FactState.GroundDistance;
    bSourceAiming = FactState.bIsAiming;
    SourceAimIntentAlpha = FactState.AimIntentAlpha;
    SourceAimIKAlpha = FactState.AimIKAlpha;
    SourceAimTargetComponentSpace = FactState.AimTargetComponentSpace;
    SourceMuzzleTransformHandRSpace = FactState.MuzzleTransformHandRSpace;
}

//------------------------------------------------------------------------------

TStaticArray<TObjectPtr<UAnimMontage> *, 5> UBBBAnimInstance::GetMontageContributions()
{
    return {
        &FullBodyMontageContribution,
        &UpperBodyMontageContribution,
        &FullBodyAdditivePreAimMontageContribution,
        &UpperBodyAdditiveMontageContribution,
        &AdditiveHitReactMontageContribution};
}

TStaticArray<const TObjectPtr<UAnimMontage> *, 5> UBBBAnimInstance::GetMontageContributions() const
{
    return {
        &FullBodyMontageContribution,
        &UpperBodyMontageContribution,
        &FullBodyAdditivePreAimMontageContribution,
        &UpperBodyAdditiveMontageContribution,
        &AdditiveHitReactMontageContribution};
}

TObjectPtr<UAnimMontage> *UBBBAnimInstance::FindMontageContribution(const FName SlotName)
{
    if (SlotName == BBBCharacterMontageSlots::FullBody)
    {
        return &FullBodyMontageContribution;
    }

    if (SlotName == BBBCharacterMontageSlots::UpperBody)
    {
        return &UpperBodyMontageContribution;
    }

    if (SlotName == BBBCharacterMontageSlots::FullBodyAdditivePreAim)
    {
        return &FullBodyAdditivePreAimMontageContribution;
    }

    if (SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
    {
        return &UpperBodyAdditiveMontageContribution;
    }

    if (SlotName == BBBCharacterMontageSlots::AdditiveHitReact)
    {
        return &AdditiveHitReactMontageContribution;
    }

    return nullptr;
}

const TObjectPtr<UAnimMontage> *UBBBAnimInstance::FindMontageContribution(const FName SlotName) const
{
    return const_cast<UBBBAnimInstance *>(this)->FindMontageContribution(SlotName);
}

bool UBBBAnimInstance::RegisterMontageContribution(
    const FName SlotName,
    UAnimMontage *Montage)
{
    TObjectPtr<UAnimMontage> *Contribution = FindMontageContribution(SlotName);
    if (!ensureMsgf(Contribution, TEXT("无法识别角色固定蒙太奇槽位 %s"), *SlotName.ToString()))
    {
        return false;
    }

    if (!Montage)
    {
        if (Contribution->Get())
        {
            Montage_Stop(0.1f, Contribution->Get());
        }
        *Contribution = nullptr;
        return true;
    }

    *Contribution = Montage;
    if (Montage_IsPlaying(Montage))
    {
        return true;
    }

    if (Montage_Play(Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, false) > 0.0f)
    {
        return true;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("角色蒙太奇注册失败 Slot=%s Montage=%s"),
        *SlotName.ToString(),
        *GetNameSafe(Montage));
    *Contribution = nullptr;
    return false;
}

void UBBBAnimInstance::ClearMontageContributions()
{
    for (TObjectPtr<UAnimMontage> *Contribution : GetMontageContributions())
    {
        UAnimMontage *Montage = Contribution->Get();
        *Contribution = nullptr;

        if (!Montage || !Montage_IsPlaying(Montage))
        {
            continue;
        }

        Montage_Stop(0.1f, Montage);
    }
}

void UBBBAnimInstance::UpdateMontageContributions()
{
    for (TObjectPtr<UAnimMontage> *Contribution : GetMontageContributions())
    {
        if (!Contribution->Get())
        {
            continue;
        }

        if (Montage_IsPlaying(Contribution->Get()))
        {
            continue;
        }

        *Contribution = nullptr;
    }
}

//------------------------------------------------------------------------------

UBBBEquipmentAnimInstance *UBBBAnimInstance::TryGetWeaponAnimInstance() const
{
    // 武器动画实例允许为空未装备时由调用方获得默认回退
    return GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance.Get();
}

FTransform UBBBAnimInstance::TryGetMuzzleTransformHandRSpace() const
{
    return GetBBBMainAnimInstanceThreadSafe()->SourceMuzzleTransformHandRSpace;
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

void UBBBAnimInstance::BindWeaponAnimInstance(UBBBEquipmentAnimInstance *InWeaponAnimInstance)
{
    GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance = InWeaponAnimInstance;
}
