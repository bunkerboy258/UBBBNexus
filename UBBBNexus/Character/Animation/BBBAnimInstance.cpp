#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterRecoilState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterWeaponHandlingState.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterEarlyTransitionAnimNotifyState.h"
#include "PoseSearch/PoseSearchResult.h"
#include "PoseSearch/PoseSearchAnimNotifies.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "PoseSearch/PoseSearchSchema.h"
#include "Components/SkeletalMeshComponent.h"
#include "Chooser.h"

bool UBBBAnimInstance::ShouldContinueMatchedTransition(
    const FMotionMatchingAnimNodeReference &Node,
    const UPoseSearchDatabase *Database)
{
    FPoseSearchBlueprintResult Match;
    bool bValid = false;
    UMotionMatchingAnimNodeLibrary::GetMotionMatchingSearchResult(Node, Match, bValid);
    const UAnimSequenceBase *Sequence = Cast<UAnimSequenceBase>(Match.SelectedAnim);
    if (!bValid || !Database || Match.SelectedDatabase != Database || !Sequence)
    {
        return false;
    }
    const float EndTime = Sequence->GetPlayLength();
    const float SearchSample = 1.0f / FMath::Max(Database->Schema ? Database->Schema->SampleRate : 30, 1);
    float EarlyTime = EndTime;
    float TailExclusionTime = EndTime;
    float BlockTime = EndTime;
    for (const FAnimNotifyEvent &Event : Sequence->Notifies)
    {
        if (Cast<UBBBCharacterEarlyTransitionAnimNotifyState>(Event.NotifyStateClass))
        {
            EarlyTime = FMath::Min(EarlyTime, Event.GetTriggerTime());
        }
        else if (Cast<UAnimNotifyState_PoseSearchExcludeFromDatabase>(Event.NotifyStateClass)
            && Event.GetEndTriggerTime() >= EndTime - SearchSample)
        {
            TailExclusionTime = FMath::Min(TailExclusionTime, Event.GetTriggerTime());
        }
        else if (Cast<UAnimNotifyState_PoseSearchBlockTransition>(Event.NotifyStateClass))
        {
            BlockTime = FMath::Min(BlockTime, Event.GetTriggerTime());
        }
    }
    if (EarlyTime < EndTime)
    {
        return Match.SelectedTime < EarlyTime;
    }
    if (TailExclusionTime < EndTime)
    {
        // 候选库会把末尾排除区间之前的最后采样点判作边界 提前交回普通库以免重选退出片段
        return Match.SelectedTime + SearchSample * 2.0f < TailExclusionTime;
    }
    return Match.SelectedTime < BlockTime;
}

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
    const FBBBCharacterAnimationFactState &FactState,
    const FBBBCharacterRecoilState &RecoilState, const FBBBCharacterWeaponHandlingState &HandlingState)
{
    SourceLifePhase = FactState.LifePhase;
    SourceLocomotionDatabaseChooser = FactState.LocomotionDatabaseChooser;
    // 将角色运行事实复制到动画实例供动画图安全读取
    SourceActorLocation = FactState.ActorLocation;
    SourceActorRotation = FactState.ActorRotation;
    SourceMeshWorldTransform = FactState.MeshWorldTransform;
    SourceVelocity = FactState.Velocity;
    SourceLastUpdateVelocity = FactState.LastUpdateVelocity;
    SourceAcceleration = FactState.Acceleration;
    SourceMaxAcceleration = FactState.MaxAcceleration;
    SourceFacingWorld = FactState.FacingWorld;
    SourceControlRotation = FactState.ControlRotation;
    SourceMovementInput = FactState.MovementInput;
    SourceBlockingSurfaceNormal = FactState.BlockingSurfaceNormal;
    SourceSecondaryBlockingSurfaceNormal = FactState.SecondaryBlockingSurfaceNormal;
    SourceRecoilAngles = RecoilState.Angles;
    SourceWeaponPoseOffsetAngles = HandlingState.PoseOffsetAngles;
    SourceWeaponAimFollowSpeed = HandlingState.AimFollowSpeed;
    SourceWeaponRecoilTime = HandlingState.RecoilTime;
    SourceWeaponBackwardRecoilStrength = HandlingState.BackwardRecoilStrength;
    SourceWeaponRecoilAnimation = HandlingState.RecoilAnimation.Get();
    SourceMovementMode = FactState.MovementMode;
    bSourceRunning = FactState.bIsRunning;
    bSourceSliding = FactState.bIsSliding;
    bSourceCrouchRequested = FactState.bCrouchRequested;
    bSourceRunRequested = FactState.bRunRequested;

    SourceGroundFriction = FactState.GroundFriction;
    SourceBrakingFriction = FactState.BrakingFriction;
    SourceBrakingFrictionFactor = FactState.BrakingFrictionFactor;
    SourceBrakingDecelerationWalking = FactState.BrakingDecelerationWalking;
    SourceMaxSpeed = FactState.MaxSpeed;
    SourceBrakingSubStepTime = FactState.BrakingSubStepTime;
    bSourceUseSeparateBrakingFriction = FactState.bUseSeparateBrakingFriction;
    SourceGravityZ = FactState.GravityZ;

    bSourceMovingOnGround = FactState.bIsMovingOnGround;
    bSourceCrouching = FactState.bIsCrouching;
    GroundDistance = FactState.GroundDistance;
    bSourceAiming = FactState.bIsAiming;
    SourceEquipmentType = FactState.EquipmentType;
    SourceCharacterGripPose = FactState.CharacterGripPose;
    SourceCharacterFireGripPose = FactState.CharacterFireGripPose;
    bSourceFire = FactState.bFire;
    SourceRightHandGripRotationOffset = FactState.RightHandGripRotationOffset;
    SourceItemType = FactState.ItemType;
    SourceAimTargetComponentSpace = FactState.AimTargetComponentSpace;
    SourceMuzzleTransformHandRSpace = FactState.MuzzleTransformHandRSpace;
    bSourceHasMuzzle = FactState.bHasMuzzle;
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
        *Contribution = nullptr;
        return true;
    }

    // 固定槽位只保留本次贡献 替换时由表现层淡出旧动画
    *Contribution = Montage;
    if (Montage_Play(Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true) > 0.0f)
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
        *Contribution = nullptr;
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

        // 暂停的攀爬尾姿仍属于状态过渡 不能按停止推进清除
        if (Montage_IsActive(Contribution->Get()))
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

UAnimSequence *UBBBAnimInstance::GetCharacterGripPose() const
{
    return GetBBBMainAnimInstanceThreadSafe()->SourceCharacterGripPose.Get();
}

FTransform UBBBAnimInstance::TryGetMuzzleTransformHandRSpace() const
{
    return GetBBBMainAnimInstanceThreadSafe()->SourceMuzzleTransformHandRSpace;
}

FVector UBBBAnimInstance::GetLeftHandIKPosition() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->GetLeftHandTargetHandRSpace();
    }

    return FVector::ZeroVector;
}

FRotator UBBBAnimInstance::GetLeftHandIKRotation() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->GetLeftHandTargetHandRSpaceRotation();
    }

    return FRotator::ZeroRotator;
}

bool UBBBAnimInstance::HasLeftHandIKTarget() const
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
