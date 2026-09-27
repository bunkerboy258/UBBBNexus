#pragma once

#include "MassLODCollectorProcessor.h"
#include "BBBMonsterLODCollectorProcessor.generated.h"

/** 启用原生视点收集 为小怪表现提供距离与视锥数据 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterLODCollectorProcessor final : public UMassLODCollectorProcessor
{
    GENERATED_BODY()

public:
    /** 注册原生批量视点收集 */
    UBBBMonsterLODCollectorProcessor();
};
