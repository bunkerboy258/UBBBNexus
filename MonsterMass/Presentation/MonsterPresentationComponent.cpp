#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterPresentationComponent.h"

#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"

UMonsterPresentationComponent::UMonsterPresentationComponent()
{
    // 表现组件不单独参与组件更新
    PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterPresentationComponent::ApplyPresentationState(
    const EMonsterState InState,
    const float InSpeed,
    const float InStateTime)
{
    // 保存 Mass 提供的只读快照，供蓝图或调试读取
    MonsterState = InState;
    MovementSpeed = FMath::Max(InSpeed, 0.0f);
    StateEnteredTime = InStateTime;

    if (bHasAppliedAnimation && LastPlayedState == InState)
    {
        return;
    }

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
    const bool bLooping = InState == EMonsterState::Idle || InState == EMonsterState::Scout || InState == EMonsterState::Chase;
    MonsterMesh->PlayAnimation(Animation, bLooping);
    LastPlayedState = InState;
    bHasAppliedAnimation = true;
}

UAnimSequenceBase* UMonsterPresentationComponent::GetAnimationForState(const EMonsterState InState) const
{
    // 根据当前状态选择对应动画
    switch (InState)
    {
        case EMonsterState::Idle:
            return IdleAnimation;

        case EMonsterState::Scout:
            return ScoutAnimation;

        case EMonsterState::Chase:
            return ChaseAnimation;

        case EMonsterState::Attack:
            return AttackAnimation;

        case EMonsterState::Hurt:
            return HurtAnimation;

        case EMonsterState::Dead:
            return DeadAnimation;
    }

    return nullptr;
}

EMonsterState UMonsterPresentationComponent::GetMonsterState() const
{
    return MonsterState;
}

float UMonsterPresentationComponent::GetMovementSpeed() const
{
    return MovementSpeed;
}

float UMonsterPresentationComponent::GetStateEnteredTime() const
{
    return StateEnteredTime;
}
