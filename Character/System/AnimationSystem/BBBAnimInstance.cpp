
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void UBBBAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    RefreshCachedReferences();
}

void UBBBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!AnimationState || !AimState)
    {

        RefreshCachedReferences();
    }
}

void UBBBAnimInstance::RefreshCachedReferences()
{

    const ABBBCharacter *Character = Cast<ABBBCharacter>(TryGetPawnOwner());
    AnimationState = Character
        ? &Character->GetAnimationState()
        : nullptr;
    AimState = Character
        ? &Character->GetAimState()
        : nullptr;
}

const FBBBCharacterAnimationState &UBBBAnimInstance::GetAnimationState() const
{
    if (AnimationState)
    {
        return *AnimationState;
    }
    static const FBBBCharacterAnimationState EmptyState;
    return EmptyState;
}
