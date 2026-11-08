#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupRequest.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupRecord.h"
#include "BBBCharacterPickupState.generated.h"

/** 拾取探测与当前交付的唯一状态 */
USTRUCT()
struct FBBBCharacterPickupState final
{
    GENERATED_BODY()
    /** 待处理的交付 */
    UPROPERTY()
    TArray<FBBBCharacterPickupGrant> PendingGrants;
    /** 每个有效掉落物的最新处理记录 */
    UPROPERTY()
    TMap<TObjectPtr<ABBBWorldDropItem>, FBBBCharacterPickupRecord> Records;
    /** 本帧已成立的领取意图 */
    UPROPERTY()
    TArray<FBBBCharacterPickupRequest> Requests;
    /** 本帧已成立的交付结果 */
    UPROPERTY()
    TArray<FBBBCharacterPickupReceipt> Receipts;
    /** 本帧投送批次序号 */
    UPROPERTY()
    uint64 DeliverySerial = 0;
    /** 下一次探测时间 */
    UPROPERTY()
    double NextProbeAt = 0;
};
