#pragma once
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBMiscDefinition.generated.h"

/** 不穿戴也不手持的杂物定义 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMiscDefinition final : public UBBBItemDefinition
{
    GENERATED_BODY()
public:
    UBBBMiscDefinition()
    {
        ItemType = EBBBItemType::Misc;
    }
};
