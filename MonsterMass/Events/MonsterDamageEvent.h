#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "Mass/ExternalSubsystemTraits.h"
#include "BBBWork/UBBBNexus/MonsterMass/Input/MonsterDamageRequest.h"

#include "MonsterDamageEvent.generated.h"

/** 等待逻辑层消费的小怪受伤事件 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterDamageEventFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 等待本帧消费的累计伤害 */
    TArray<FMonsterDamageRequest> PendingRequests;

    /** 最近一次实际扣血的完整来源信息 */
    FMonsterDamageRequest LastDamageRequest;

    /** 伤害处理后交由状态处理器消费的受伤事实 */
    bool bReceivedDamage = false;
};

/** 只有受伤事件允许按需分配队列 避免为每个未受击实体预留大量请求槽位 */
template<>
struct TMassFragmentTraits<FMonsterDamageEventFragment> final
{
    /** 使用结构体原生复制与析构维护队列所有权 */
    enum
    {
        /** 明确允许事件队列持有动态存储 */
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
