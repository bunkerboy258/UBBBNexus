#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemBarState.generated.h"
class AActor;

/** 两类快捷选择及唯一目标主手结果 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemBarState final
{
    GENERATED_BODY()
    /** 装备快捷位置数量 */
    int32 QuickAccessSlotCount = 0;
    /** 当前主手选择的真实位置 */
    int32 SelectedSlot = INDEX_NONE;
    /** 当前选择的消耗品快捷索引 空格也保留选择 */
    int32 SelectedConsumableSlot = 0;
    /** 需要恢复的原装备稳定身份 */
    FGuid PreviousEquipment;
    /** 临时使用结束时恢复装备 */
    bool bReturnAfterUse = false;
    /** 挂接成功后只提交一次主行为 */
    bool bUseWhenReady = false;
    /** 本帧唯一目标主手物品 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "目标主手物品"))
    TObjectPtr<AActor> DesiredMainHandItem = nullptr;
    /** 选择或目标发生变化的版本 */
    int32 Revision = 0;
};
