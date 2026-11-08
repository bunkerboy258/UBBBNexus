#pragma once
#include "CoreMinimal.h"
class UBBBItemDefinition;
class ABBBCharacter;
#include "BBBWorldDropSnapshot.generated.h"
/** 世界掉落物已成立的完整当前结果 */
USTRUCT()
struct FBBBWorldDropSnapshot final
{
    GENERATED_BODY()
    /** 本份世界掉落物身份 */
    UPROPERTY()
    FGuid DropId;
    /** 当前交付序号 */
    UPROPERTY()
    uint64 Operation = 0;
    /** 当前结果版本 */
    UPROPERTY()
    uint64 Revision = 0;
    /** 物品静态定义 */
    UPROPERTY()
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;
    /** 整次交付数量 */
    UPROPERTY()
    int32 Quantity = 0;
    /** 当前交付收件人 */
    UPROPERTY()
    TObjectPtr<ABBBCharacter> Recipient = nullptr;
    /** 是否已经落地 */
    UPROPERTY()
    bool bLanded = false;
    /** 是否正在交付 */
    UPROPERTY()
    bool bClaimed = false;
    /** 是否已经领取 */
    UPROPERTY()
    bool bConsumed = false;

};
