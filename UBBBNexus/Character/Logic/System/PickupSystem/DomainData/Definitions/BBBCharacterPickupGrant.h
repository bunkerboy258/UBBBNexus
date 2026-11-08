#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterPickupGrant.generated.h"
class ABBBWorldDropItem;
/** 掉落物已经确定收件人的一次交付 */
USTRUCT()
struct FBBBCharacterPickupGrant final
{
    GENERATED_BODY()
    /** 交付来源 */
    UPROPERTY()
    TObjectPtr<ABBBWorldDropItem> Drop = nullptr;
    /** 来源实例身份 */
    UPROPERTY()
    FGuid DropId;
    /** 来源内递增的交付序号 */
    UPROPERTY()
    uint64 Operation = 0;
    /** 待获得的型号 */
    UPROPERTY()
    FName ItemId;
    /** 必须整次接受的数量 */
    UPROPERTY()
    int32 Quantity = 0;
};
