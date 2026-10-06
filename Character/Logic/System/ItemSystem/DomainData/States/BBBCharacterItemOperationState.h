#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterItemOperationState.generated.h"

/** 输入解析发布的待消费操作及最近一次完成结果 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemOperationState final
{
    GENERATED_BODY()

    /** 待创建并入包的装备定义 */
    TArray<FName> PendingEquipmentIds;

    /** 待移动操作的起始槽位 与目标数组一一对应 */
    TArray<int32> PendingMoveSources;

    /** 待移动操作的目标槽位 */
    TArray<int32> PendingMoveTargets;

    /** 待应用的快捷槽位选择 */
    TArray<int32> PendingSelectedSlots;

    /** 每完成一项操作递增 输入接受不等于操作完成 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "操作结果版本"))
    int32 Revision = 0;

    /** 本帧成功操作数量 下一批操作完成时更新 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "成功操作数量"))
    int32 SucceededCount = 0;

    /** 本帧失败操作数量 下一批操作完成时更新 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "失败操作数量"))
    int32 RejectedCount = 0;
};
