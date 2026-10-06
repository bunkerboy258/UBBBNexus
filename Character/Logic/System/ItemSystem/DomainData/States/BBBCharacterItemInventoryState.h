#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Definitions/BBBCharacterItem.h"
#include "BBBCharacterItemInventoryState.generated.h"

/** 所有格子规则相同的唯一真实背包 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemInventoryState final
{
    GENERATED_BODY()

    /** 全部物品槽位 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "背包槽位"))
    TArray<FBBBCharacterItem> BackpackSlots;

    /** 背包内容发生变化时递增 供表现层观察 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "背包版本"))
    int32 Revision = 0;
};
