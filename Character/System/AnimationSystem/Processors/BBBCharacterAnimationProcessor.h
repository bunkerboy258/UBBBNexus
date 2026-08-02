
#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
struct FBBBAimRuntimeData;
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterAnimationState;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;

//消费蒙太奇队列并更新表现开关
class ABBB_EVAC_API FBBBCharacterAnimationProcessor final
{
public:

    void Update(
        USkeletalMeshComponent &CharacterMesh,
        const FBBBCharacterAnimationCommands &AnimationCommands,
        FBBBCharacterAnimationState &AnimationState,
        const FBBBIntentRuntimeData &IntentData,
        const FBBBFacingRuntimeData &FacingData,
        const FBBBAimRuntimeData &AimData) const;
};
