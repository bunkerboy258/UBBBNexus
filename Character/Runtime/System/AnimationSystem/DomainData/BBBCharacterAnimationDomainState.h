#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterAnimationState.h"
#include "BBBCharacterAnimationDomainState.generated.h"

class FBBBCharacterAnimationActionProcessor;
class FBBBCharacterAnimationFactProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBCharacterAnimationSystem;
class FBBBCharacterParseSystem;

/** 角色动画状态的唯一持有者 */
USTRUCT()
struct FBBBCharacterAnimationDomainState final
{
    GENERATED_BODY()

public:
    /** @return 角色当前动画状态 */
    const FBBBCharacterAnimationState &ReadAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class FBBBCharacterAnimationActionProcessor;
    friend class FBBBCharacterAnimationFactProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBCharacterAnimationSystem;
    friend class FBBBCharacterParseSystem;

    /** 角色当前动画状态 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationState AnimationState;
};
