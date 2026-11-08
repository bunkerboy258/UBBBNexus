#pragma once
#include "CoreMinimal.h"

#include "BBBCharacterItemAcquisitionKey.generated.h"

/** 获得请求的来源与离散操作身份 */
USTRUCT()
struct FBBBCharacterItemAcquisitionKey final
{
    GENERATED_BODY()
    /** 来源实例身份 无对应结果需求时为空 */
    UPROPERTY()
    FGuid SourceId;
    /** 来源实例中的操作序号 */
    UPROPERTY()
    uint64 Operation = 0;
};
