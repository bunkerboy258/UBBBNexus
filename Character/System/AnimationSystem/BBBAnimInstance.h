
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationState.h"
#include "BBBAnimInstance.generated.h"

UCLASS()
class ABBB_EVAC_API UBBBAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
public:

    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    EBBBLocomotionState GetLocomotionState() const
    {
        return GetAnimationState().LocomotionState;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetNormalizedMoveRight() const
    {
        return GetAnimationState().MoveInput.X;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetNormalizedMoveForward() const
    {
        return GetAnimationState().MoveInput.Y;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetSmoothedMoveRight() const
    {
        return GetAnimationState().SmoothedMoveInput.X;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    float GetSmoothedMoveForward() const
    {
        return GetAnimationState().SmoothedMoveInput.Y;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsTurningInPlaceLeft() const
    {
        return GetAnimationState().bIsTurningInPlaceLeft;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Locomotion", meta = (BlueprintThreadSafe))
    bool IsTurningInPlaceRight() const
    {
        return GetAnimationState().bIsTurningInPlaceRight;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Weapon", meta = (BlueprintThreadSafe))
    bool WantsToFire() const
    {
        return GetAnimationState().bWantsFire;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Weapon", meta = (BlueprintThreadSafe))
    bool WantsToAim() const
    {
        return GetAnimationState().bWantsAim;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FVector GetAimTargetComponentSpace() const
    {
        return GetAnimationState().AimTargetComponentSpace;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    FTransform GetAimSourceLocalTransform() const
    {
        return GetAnimationState().AimSourceLocalTransform;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimTarget() const
    {
        return GetAnimationState().bHasValidAimTarget;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|Aim", meta = (BlueprintThreadSafe))
    bool HasValidAimSource() const
    {
        return GetAnimationState().bHasValidAimSource;
    }


    UFUNCTION(BlueprintPure, Category = "BBB|HandIK", meta = (BlueprintThreadSafe))
    FVector GetLeftHandIKTargetLocation() const
    {
        return GetAnimationState().LeftHandTargetRightHandSocketSpace.GetLocation();
    }

    UFUNCTION(BlueprintPure, Category = "BBB|HandIK", meta = (BlueprintThreadSafe))
    bool HasValidLeftHandTarget() const
    {
        return GetAnimationState().bHasValidLeftHandTarget;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|IKAlpha", meta = (BlueprintThreadSafe))
    float GetAimIKAlpha() const
    {
        return GetAnimationState().AimIKAlpha;
    }

    UFUNCTION(BlueprintPure, Category = "BBB|IKAlpha", meta = (BlueprintThreadSafe))
    float GetLeftHandIKAlpha() const
    {
        return GetAnimationState().LeftHandIKAlpha;
    }
protected:

    void RefreshCachedReferences();

    const FBBBCharacterAnimationState &GetAnimationState() const;

    const FBBBCharacterAnimationState *AnimationState = nullptr;
};
