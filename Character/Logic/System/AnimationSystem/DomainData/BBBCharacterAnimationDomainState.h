#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationLayerState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "BBBCharacterAnimationDomainState.generated.h"

class FBBBCharacterAnimationFactProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBCharacterAnimationSystem;
class FBBBCharacterParseSystem;

/** 角色动画领域全部状态的唯一直接持有者 */
USTRUCT()
struct FBBBCharacterAnimationDomainState final
{
    GENERATED_BODY()

public:
    /** @return 角色动画事实状态 */
    const FBBBCharacterAnimationFactState &ReadAnimationFactState() const
    {
        return AnimationFactState;
    }

    /** @return 角色动画层状态 */
    const FBBBCharacterAnimationLayerState &ReadAnimationLayerState() const
    {
        return AnimationLayerState;
    }

    /** @return 角色动画蒙太奇状态 */
    const FBBBCharacterAnimationMontageState &ReadAnimationMontageState() const
    {
        return AnimationMontageState;
    }

private:
    friend class FBBBCharacterAnimationFactProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBCharacterAnimationSystem;
    friend class FBBBCharacterParseSystem;

    /** 角色动画事实状态 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationFactState AnimationFactState;

    /** 角色动画层状态 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationLayerState AnimationLayerState;

    /** 角色动画蒙太奇请求状态 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationMontageState AnimationMontageState;
};
