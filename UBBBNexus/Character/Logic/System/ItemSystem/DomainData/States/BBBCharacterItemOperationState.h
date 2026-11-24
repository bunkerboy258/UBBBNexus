#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Definitions/BBBCharacterItemAcquisitionResult.h"
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
    /** 与获得请求逐项对应的可选来源身份 */
    TArray<FBBBCharacterItemAcquisitionKey> PendingAcquisitionKeys;
    /** 本帧已完成的有来源获得结果 */
    UPROPERTY()
    TArray<FBBBCharacterItemAcquisitionResult> AcquisitionResults;
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
    /** 本帧是否请求按持久快照整体还原背包 */
    bool bPendingRestore = false;
    /** 还原目标槽位索引 */
    TArray<int32> PendingRestoreSlots;
    /** 还原目标实例身份 */
    TArray<FGuid> PendingRestoreInstances;
    /** 还原目标物品型号 */
    TArray<FName> PendingRestoreItemIds;
    /** 还原目标堆叠数量 */
    TArray<int32> PendingRestoreQuantities;
    /** 还原后选中的装备快捷槽位 */
    int32 PendingRestoreWeaponSlot = INDEX_NONE;
    /** 还原后选中的消耗品快捷槽位 */
    int32 PendingRestoreConsumableSlot = INDEX_NONE;
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
