
#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
struct FBBBAimAnimationConfig;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationState;

//把瞄准逻辑转换为动画IK数据
class FBBBCharacterAimPresentationProcessor final
{
public:

    void Update(
        USkeletalMeshComponent &CharacterMesh,
        float DeltaSeconds,
        const FBBBAimConfig &AimConfig,
        const FBBBAimAnimationConfig &AnimationConfig,
        const FBBBAimRuntimeData &AimData,
        FBBBAnimationRuntimeData &AnimationData,
        FBBBCharacterAnimationState &AnimationState) const;
private:

    //带速度项的平滑阻尼跟随目标
    FVector SmoothTarget(
        const FVector &Current,
        const FVector &Target,
        FVector &Velocity,
        float SmoothTime,
        float DeltaSeconds) const;
};
