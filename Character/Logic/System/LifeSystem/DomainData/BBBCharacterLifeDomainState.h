#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterHitState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterDamageDeliveryState.h"
#include "BBBCharacterLifeDomainState.generated.h"

/** 角色生命领域全部状态的唯一持有者 */
USTRUCT()
struct FBBBCharacterLifeDomainState final
{
    GENERATED_BODY()

    /** @return 当前生命结果 */
    const FBBBCharacterLifeState &ReadLifeState() const
    {
        return LifeState;
    }

    /** @return 最近有效命中事实 */
    const FBBBCharacterHitState &ReadHitState() const
    {
        return HitState;
    }

    /** @return 待消费输入 */
    const FBBBCharacterLifeInputState &ReadLifeInputState() const
    {
        return LifeInputState;
    }

    /** @return 本帧需要投送的命中事实 */
    const FBBBCharacterDamageDeliveryState &ReadDamageDeliveryState() const
    {
        return DamageDeliveryState;
    }

  private:
    friend class FBBBCharacterLifeProcessor;
    friend class FBBBCharacterParseSystem;

    /** 当前生命结果 */
    FBBBCharacterLifeState LifeState;

    /** 当前有效命中事实 */
    FBBBCharacterHitState HitState;

    /** 待处理输入 */
    FBBBCharacterLifeInputState LifeInputState;

    /** 本帧命中投送结果 */
    FBBBCharacterDamageDeliveryState DamageDeliveryState;
};
