#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterAnimInstance.h"

UBBBMonsterPresentationComponent::UBBBMonsterPresentationComponent()
{
    // 表现组件不单独参与组件更新
    PrimaryComponentTick.bCanEverTick = false;
}

void UBBBMonsterPresentationComponent::ApplyPresentationState(
    const EBBBMonsterBehavior InState,
    const float InSpeed,
    const float InStateTime,
    const uint32 InActionId,
    const float InActionProgress)
{
    const bool bNewAction = !bHasAppliedAnimation || LastPlayedState != InState || LastActionId != InActionId || StateEnteredTime != InStateTime;

    // 保存 Mass 提供的只读快照，供蓝图或调试读取
    BBBMonsterBehavior = InState;
    MovementSpeed = FMath::Max(InSpeed, 0.0f);
    StateEnteredTime = InStateTime;

    // 仅在状态切换时重新播放动画，避免每帧重置时间轴
    UAnimSequenceBase* const Animation = GetAnimationForState(InState);

    if (!ensureMsgf(Animation != nullptr, TEXT("[UBBBM]Monster presentation requires an animation for state %d"), static_cast<int32>(InState)))
    {
        return;
    }

    USkeletalMeshComponent* const MonsterMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();

    if (!ensureMsgf(MonsterMesh != nullptr, TEXT("[UBBBM]Monster presentation requires a skeletal mesh component")))
    {
        return;
    }

    // 移动相关状态循环播放，其余状态只播放一次
    const bool bLooping = InState == EBBBMonsterBehavior::Idle || InState == EBBBMonsterBehavior::Scout || InState == EBBBMonsterBehavior::Chase;
    if (bNewAction)
    {
        if (!ensureMsgf(Cast<UBBBMonsterAnimInstance>(MonsterMesh->GetAnimInstance()) != nullptr,
            TEXT("[UBBBM]Configure a BBBMonsterAnimInstance animation blueprint on the presentation mesh; SingleNode playback is not supported")))
        {
            return;
        }
    }

    // 非循环动作由逻辑时间定位 禁用通知触发 避免不可见时漏伤或重复伤害
    if (!bLooping)
    {
        ActionProgress = FMath::Clamp(InActionProgress, 0.0f, 1.0f);
    }

    LastActionId = InActionId;
    LastPlayedState = InState;
    bHasAppliedAnimation = true;
}

UAnimSequenceBase* UBBBMonsterPresentationComponent::SelectAnimationForAction(const EBBBMonsterBehavior InState, const uint32 InActionId, const uint32 InSeed) const
{
    const TArray<TObjectPtr<UAnimSequenceBase>>* Variants = nullptr;
    switch (InState)
    {
        case EBBBMonsterBehavior::Idle:
            Variants = &IdleAnimationVariants;
            break;

        case EBBBMonsterBehavior::Chase:
            Variants = &ChaseAnimationVariants;
            break;

        case EBBBMonsterBehavior::Attack:
            Variants = &AttackAnimationVariants;
            break;

        case EBBBMonsterBehavior::Hurt:
            Variants = &HurtAnimationVariants;
            break;

        case EBBBMonsterBehavior::Dead:
            Variants = &DeadAnimationVariants;
            break;

        case EBBBMonsterBehavior::Scout:
            break;
    }

    const uint32 Count = Variants ? static_cast<uint32>(Variants->Num()) + 1 : 1;
    const uint32 Choice = (InSeed % Count + InActionId % Count) % Count;
    UAnimSequenceBase* const Selected = Choice == 0 ? GetAnimationForState(InState) : (*Variants)[Choice - 1].Get();
    if (!ensureMsgf(Selected != nullptr, TEXT("[UBBBM]Remove null animation variants from the presentation configuration")))
    {
        return nullptr;
    }

    return Selected;
}

UAnimSequenceBase* UBBBMonsterPresentationComponent::GetAnimationForState(const EBBBMonsterBehavior InState) const
{
    // 根据当前状态选择对应动画
    switch (InState)
    {
        case EBBBMonsterBehavior::Idle:
            return IdleAnimation;

        case EBBBMonsterBehavior::Scout:
            return ScoutAnimation;

        case EBBBMonsterBehavior::Chase:
            return ChaseAnimation;

        case EBBBMonsterBehavior::Attack:
            return AttackAnimation;

        case EBBBMonsterBehavior::Hurt:
            return HurtAnimation;

        case EBBBMonsterBehavior::Dead:
            return DeadAnimation;
    }

    return nullptr;
}

EBBBMonsterBehavior UBBBMonsterPresentationComponent::GetBBBMonsterBehavior() const
{
    return BBBMonsterBehavior;
}

float UBBBMonsterPresentationComponent::GetMovementSpeed() const
{
    return MovementSpeed;
}

float UBBBMonsterPresentationComponent::GetStateEnteredTime() const
{
    return StateEnteredTime;
}
