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
    /** 最近选择的投掷位置 */
    int32 SelectedThrowableSlot = 0;
    /** 本帧选中的消耗品稳定身份 不替换装备主手 */
    FGuid DesiredConsumableIdentity;
    /** 本帧选中的消耗品静态定义 */
    UPROPERTY(Transient)
    TObjectPtr<class UBBBConsumableDefinition> DesiredConsumableDefinition = nullptr;
    /** 已批准的当次使用请求 */
    bool bStartUse = false;
    /** 已批准的起始阶段 */
    uint8 UseStage = 0;
    /** 本帧唯一目标主手物品 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "目标主手物品"))
    TObjectPtr<AActor> DesiredMainHandItem = nullptr;
    /** 选择或目标发生变化的版本 */
    int32 Revision = 0;
};
