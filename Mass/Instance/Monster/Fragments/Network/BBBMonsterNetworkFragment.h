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

    /** 静态配置引用 */
    UPROPERTY()
    TWeakObjectPtr<UBBBMonsterDefinition> Definition;
};
