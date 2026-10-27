#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterNetworkFragment.generated.h"

class UBBBMonsterDefinition;

/** 小怪跨机身份与已接收版本 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterNetworkFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 每次出生独立分配 不复用本地实体下标 */
    FGuid InstanceId;

    /** 最新接收的事实版本 */
    uint32 ReceivedRevision = 0;

    /** 当前远端爬行转换的本机开始时刻 只用于恢复进度 不裁决腿部损毁 */
    float CrawlStartedAt = -1.0f;

    /** 转换后的本机时刻允许为负 必须单独表达是否收到该事实 */
    bool bHasCrawlTime = false;

    /** 已收到的远端死亡开始时刻 本机伤害字典成立死亡后才使用 */
    float DeathStartedAt = -1.0f;

    /** 死亡时刻已经收到 不以本机时刻是否非负判断有效性 */
    bool bHasDeathTime = false;

    /** 最新本机贡献已经交接给可靠发送流程后才允许回收 */
    bool bDamageSubmitted = false;

    /** 静态配置引用 */
    UPROPERTY()
    TWeakObjectPtr<UBBBMonsterDefinition> Definition;
};
