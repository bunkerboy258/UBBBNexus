#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBFireRuntimeData.generated.h"

/** 开火操作域运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBFireRuntimeData final : public UObject
{
    GENERATED_BODY()

public:
    /** 上次成功开火时间 */
    UPROPERTY(BlueprintReadOnly)
    float LastFireTime = -1000.0f;
};
