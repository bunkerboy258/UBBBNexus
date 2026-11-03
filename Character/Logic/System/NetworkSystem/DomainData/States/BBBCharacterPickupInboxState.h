#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupRequest.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupReceipt.h"
#include "BBBCharacterPickupInboxState.generated.h"

/** 经角色连接接收并等待投送的拾取输入 */
USTRUCT()
struct FBBBCharacterPickupInboxState final
{
    GENERATED_BODY()
    /** 待交给世界掉落物的领取请求 */
    UPROPERTY()
    TArray<FBBBCharacterPickupRequest> Requests;
    /** 待交给世界掉落物的交付结果 */
    UPROPERTY()
    TArray<FBBBCharacterPickupReceipt> Receipts;
    /** 本机已发送的角色投送批次 */
    UPROPERTY()
    uint64 SentSerial = 0;
};
