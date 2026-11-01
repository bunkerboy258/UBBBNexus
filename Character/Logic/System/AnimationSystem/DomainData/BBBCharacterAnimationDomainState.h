#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationFactState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationLayerState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterLifeAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterRecoilState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterWeaponHandlingState.h"
#include "BBBCharacterAnimationDomainState.generated.h"

class FBBBCharacterAnimationFactProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBCharacterAnimationSystem;
class FBBBCharacterParseSystem;
class FBBBCharacterMuzzleProcessor;

/** 角色动画领域全部状态的唯一直接持有者 */
USTRUCT()
struct FBBBCharacterAnimationDomainState final
{
    GENERATED_BODY()

public:
    /** @return 生命阶段动画的应用进度 */
    const FBBBCharacterLifeAnimationState &ReadLifeAnimationState() const
    {
        return LifeAnimationState;
    }

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


    /** @return 当前后坐力段与结果 */
    const FBBBCharacterRecoilState &ReadRecoilState() const { return RecoilState; }

    /** @return 当前持枪动画领域结果 */
    const FBBBCharacterWeaponHandlingState &ReadWeaponHandlingState() const { return WeaponHandlingState; }

private:
    friend class FBBBCharacterRecoilProcessor;
    friend class FBBBCharacterWeaponHandlingProcessor;
    /** 唯一后坐力运行状态 */
    FBBBCharacterRecoilState RecoilState;
    /** 唯一持枪动画领域结果 */
    FBBBCharacterWeaponHandlingState WeaponHandlingState;
    friend class FBBBCharacterLifeAnimationProcessor;
    /** 生命阶段动画的应用进度 */
    UPROPERTY(Transient)
    FBBBCharacterLifeAnimationState LifeAnimationState;

    friend class FBBBCharacterAnimationFactProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBCharacterAnimationSystem;
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterMuzzleProcessor;

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
