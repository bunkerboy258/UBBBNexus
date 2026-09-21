#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentAnimInstance.h"
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
    SourceGait = FactState.Gait;

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
}

//------------------------------------------------------------------------------

TStaticArray<FBBBCharacterMontageSlot *, 5> UBBBAnimInstance::GetMontageSlots()
{
    return {
        &FullBodyMontageSlot,
        &UpperBodyMontageSlot,
        &FullBodyAdditivePreAimMontageSlot,
        &UpperBodyAdditiveMontageSlot,
        &AdditiveHitReactMontageSlot};
}

TStaticArray<const FBBBCharacterMontageSlot *, 5> UBBBAnimInstance::GetMontageSlots() const
{
    return {
        &FullBodyMontageSlot,
        &UpperBodyMontageSlot,
        &FullBodyAdditivePreAimMontageSlot,
        &UpperBodyAdditiveMontageSlot,
        &AdditiveHitReactMontageSlot};
}

FBBBCharacterMontageSlot *UBBBAnimInstance::FindMontageSlot(const FName SlotName)
{
    if (SlotName == BBBCharacterMontageSlots::FullBody)
    {
        return &FullBodyMontageSlot;
    }

    if (SlotName == BBBCharacterMontageSlots::UpperBody)
    {
        return &UpperBodyMontageSlot;
    }

    if (SlotName == BBBCharacterMontageSlots::FullBodyAdditivePreAim)
    {
        return &FullBodyAdditivePreAimMontageSlot;
    }

    if (SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
    {
        return &UpperBodyAdditiveMontageSlot;
    }

    if (SlotName == BBBCharacterMontageSlots::AdditiveHitReact)
    {
        return &AdditiveHitReactMontageSlot;
    }

    return nullptr;
}

const FBBBCharacterMontageSlot *UBBBAnimInstance::FindMontageSlot(const FName SlotName) const
{
    return const_cast<UBBBAnimInstance *>(this)->FindMontageSlot(SlotName);
}

bool UBBBAnimInstance::SubmitMontageSlot(
    const FName SlotName,
    UAnimMontage &Montage,
    const float PlayRate,
    const int32 Sequence,
    const bool bReload)
{
    FBBBCharacterMontageSlot *TargetSlot = FindMontageSlot(SlotName);
    if (!ensureMsgf(TargetSlot, TEXT("无法识别角色固定蒙太奇槽位 %s"), *SlotName.ToString()))
    {
        return false;
    }

    uint64 SharedRevision = 0;
    bool bSharedRevisionSubmitted = false;
    for (const FBBBCharacterMontageSlot *ExistingSlot : GetMontageSlots())
    {
        if (ExistingSlot->Montage == &Montage
            && ExistingSlot->Sequence == Sequence
            && ExistingSlot->Revision != 0)
        {
            SharedRevision = ExistingSlot->Revision;
            bSharedRevisionSubmitted = ExistingSlot->bSubmitted;
            break;
        }
    }

    if (SharedRevision != 0)
    {
        if (TargetSlot->Revision != SharedRevision)
        {
            ReleaseMontageSlot(*TargetSlot);
        }

        TargetSlot->Montage = &Montage;
        TargetSlot->PlayRate = PlayRate;
        TargetSlot->Sequence = Sequence;
        TargetSlot->Revision = SharedRevision;
        TargetSlot->bReload = bReload;
        TargetSlot->bSubmitted = bSharedRevisionSubmitted;
        return true;
    }

    for (const FBBBCharacterMontageSlot *ExistingSlot : GetMontageSlots())
    {
        if (!ExistingSlot->Montage
            || ExistingSlot->Revision == 0
            || ExistingSlot->Montage->GetGroupName() != Montage.GetGroupName())
        {
            continue;
        }

        const uint64 ConflictingRevision = ExistingSlot->Revision;
        ClearMontageRevision(ConflictingRevision, true);
    }

    ReleaseMontageSlot(*TargetSlot);
    TargetSlot->Montage = &Montage;
    TargetSlot->PlayRate = PlayRate;
    TargetSlot->Sequence = Sequence;
    TargetSlot->Revision = NextMontageRevision++;
    TargetSlot->bReload = bReload;
    TargetSlot->bSubmitted = false;
    return true;
}

void UBBBAnimInstance::InvalidateMontageSlots(
    const bool bEquipmentSwitchPending,
    const int32 CancelledReloadSequence)
{
    for (FBBBCharacterMontageSlot *Slot : GetMontageSlots())
    {
        if (!Slot->Montage)
        {
            continue;
        }

        const bool bCancelledReload = Slot->bReload
            && CancelledReloadSequence != INDEX_NONE
            && Slot->Sequence == CancelledReloadSequence;
        if (!bEquipmentSwitchPending && !bCancelledReload)
        {
            continue;
        }

        ReleaseMontageSlot(*Slot);
    }
}

void UBBBAnimInstance::UpdateApprovedMontages()
{
    for (const FBBBCharacterMontageSlot *Slot : GetMontageSlots())
    {
        if (!Slot->Montage || Slot->Revision == 0 || !Slot->bSubmitted)
        {
            continue;
        }

        if (Montage_IsPlaying(Slot->Montage))
        {
            continue;
        }

        const uint64 CompletedRevision = Slot->Revision;
        ClearMontageRevision(CompletedRevision, false);
    }

    for (FBBBCharacterMontageSlot *Slot : GetMontageSlots())
    {
        if (!Slot->Montage || Slot->Revision == 0 || Slot->bSubmitted)
        {
            continue;
        }

        bool bRevisionSubmitted = false;
        for (const FBBBCharacterMontageSlot *RelatedSlot : GetMontageSlots())
        {
            if (RelatedSlot->Revision == Slot->Revision && RelatedSlot->bSubmitted)
            {
                bRevisionSubmitted = true;
                break;
            }
        }

        if (!bRevisionSubmitted && Montage_Play(Slot->Montage, Slot->PlayRate) <= 0.0f)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("角色蒙太奇提交失败 Montage=%s Revision=%llu"),
                *GetNameSafe(Slot->Montage),
                Slot->Revision);
            const uint64 FailedRevision = Slot->Revision;
            ClearMontageRevision(FailedRevision, false);
            continue;
        }

        const uint64 SubmittedRevision = Slot->Revision;
        for (FBBBCharacterMontageSlot *RelatedSlot : GetMontageSlots())
        {
            if (RelatedSlot->Revision == SubmittedRevision)
            {
                RelatedSlot->bSubmitted = true;
            }
        }
    }
}

void UBBBAnimInstance::ClearMontageRevision(
    const uint64 Revision,
    const bool bStopMontage)
{
    UAnimMontage *MontageToStop = nullptr;
    bool bWasSubmitted = false;
    for (const FBBBCharacterMontageSlot *Slot : GetMontageSlots())
    {
        if (Slot->Revision != Revision)
        {
            continue;
        }

        MontageToStop = Slot->Montage;
        bWasSubmitted |= Slot->bSubmitted;
    }

    if (bStopMontage
        && bWasSubmitted
        && MontageToStop
        && Montage_IsPlaying(MontageToStop))
    {
        Montage_Stop(0.1f, MontageToStop);
    }

    for (FBBBCharacterMontageSlot *Slot : GetMontageSlots())
    {
        if (Slot->Revision == Revision)
        {
            *Slot = FBBBCharacterMontageSlot();
        }
    }
}

void UBBBAnimInstance::ReleaseMontageSlot(FBBBCharacterMontageSlot &Slot)
{
    if (Slot.Revision == 0)
    {
        Slot = FBBBCharacterMontageSlot();
        return;
    }

    const uint64 ReleasedRevision = Slot.Revision;
    UAnimMontage *ReleasedMontage = Slot.Montage;
    const bool bReleasedSubmitted = Slot.bSubmitted;
    Slot = FBBBCharacterMontageSlot();

    for (const FBBBCharacterMontageSlot *RemainingSlot : GetMontageSlots())
    {
        if (RemainingSlot->Revision == ReleasedRevision)
        {
            return;
        }
    }

    if (bReleasedSubmitted
        && ReleasedMontage
        && Montage_IsPlaying(ReleasedMontage))
    {
        Montage_Stop(0.1f, ReleasedMontage);
    }
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
