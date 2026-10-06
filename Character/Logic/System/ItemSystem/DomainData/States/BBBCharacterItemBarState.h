#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterItemBarState.generated.h"

class AActor;

/** 物品栏上层选择及供装备系统消费的结果 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemBarState final
{
    GENERATED_BODY()

    /** 可用于快捷选择的前序槽位数量 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "快捷槽位数量"))
    int32 QuickAccessSlotCount = 0;

    /** 当前选中的前序槽位 INDEX_NONE 表示未选择 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "选中槽位"))
    int32 SelectedSlot = INDEX_NONE;

    /** 当前槽位对应的目标主手物品 空引用表示空手 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "目标主手物品"))
    TObjectPtr<AActor> DesiredMainHandItem = nullptr;

    /** 选择或目标变化时递增 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "物品栏版本"))
    int32 Revision = 0;
};
