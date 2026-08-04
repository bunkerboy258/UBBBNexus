#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BBBItemOperation.generated.h"

/** 物品操作域配置基类 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBItemOperation : public UObject
{
    GENERATED_BODY()
};
