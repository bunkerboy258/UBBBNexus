#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemOperationState.generated.h"

/** 角色物品输入及完成结果 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemOperationState final
{
    GENERATED_BODY()
    /** 待获得的型号 */
    TArray<FName> PendingItemIds;
    /** 与型号对应的整次获取数量 */
    TArray<int32> PendingQuantities;
    /** 待移动的源位置 */
    TArray<int32> PendingMoveSources;
    /** 待移动的目标位置 */
    TArray<int32> PendingMoveTargets;
    /** 防止排队后作用于另一件物品的源身份 */
    TArray<FGuid> PendingMoveInstances;
    /** 待选择的真实快捷位置 */
    TArray<int32> PendingSelectedSlots;
    /** 待扣减的真实物品身份 */
    TArray<FGuid> PendingConsumeInstances;
    /** 与扣减身份对应的数量 */
    TArray<int32> PendingConsumeQuantities;
    /** 已完成使用动作的实例 */
    TArray<FGuid> PendingFinishedInstances;
    /** 本帧临时使用当前消耗品的请求 */
    bool bQuickUse = false;
    /** 操作完成版本 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "操作版本"))
    int32 Revision = 0;
    /** 最近一次批次的成功数量 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "成功数量"))
    int32 SucceededCount = 0;
    /** 最近一次批次的拒绝数量 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "拒绝数量"))
    int32 RejectedCount = 0;
};
