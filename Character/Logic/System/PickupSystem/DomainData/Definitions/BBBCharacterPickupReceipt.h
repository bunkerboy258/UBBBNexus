#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupGrant.h"
#include "BBBCharacterPickupReceipt.generated.h"

/** 角色对一次交付的已成立入包结果 */
USTRUCT()
struct FBBBCharacterPickupReceipt final
{
    GENERATED_BODY()
    /** 对应的交付 */
    UPROPERTY()
    FBBBCharacterPickupGrant Grant;
    /** 整次数量是否已入包 */
    UPROPERTY()
    bool bSucceeded = false;
};
