#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterPresentationComponent.h"

#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"

UMonsterPresentationComponent::UMonsterPresentationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterPresentationComponent::ApplyPresentationState(
    const EMonsterState InState,
    const float InSpeed,
    const float InStateTime)
{
    MonsterState = InState;
    MovementSpeed = FMath::Max(InSpeed, 0.0f);
    StateEnteredTime = InStateTime;

    if (bHasAppliedAnimation && LastPlayedState == InState)
    {
        return;
    }

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

    const bool bLooping = InState == EMonsterState::Idle || InState == EMonsterState::Scout || InState == EMonsterState::Chase;
    MonsterMesh->PlayAnimation(Animation, bLooping);
    LastPlayedState = InState;
    bHasAppliedAnimation = true;
}

UAnimSequenceBase* UMonsterPresentationComponent::GetAnimationForState(const EMonsterState InState) const
{
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
