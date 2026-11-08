#pragma once
#include "CoreMinimal.h"

#include "BBBWorldDropNetworkState.generated.h"
/** 掉落物结果复制与单次交付发送基准 */
USTRUCT()
struct FBBBWorldDropNetworkState final
{
    GENERATED_BODY()
    /** 已发布的结果版本 */
    UPROPERTY()
    uint64 PublishedRevision = 0;
    /** 已向收件人发送的交付序号 */
    UPROPERTY()
    uint64 GrantedOperation = 0;

};
