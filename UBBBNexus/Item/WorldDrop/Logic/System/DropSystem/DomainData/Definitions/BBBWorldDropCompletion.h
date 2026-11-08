#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupReceipt.h"
#include "BBBWorldDropCompletion.generated.h"
class ABBBCharacter;
/** 带实际发送角色身份的交付结果 */
USTRUCT()
struct FBBBWorldDropCompletion final
{
    GENERATED_BODY()
    /** 经角色连接确定的发送者 */
    UPROPERTY()
    TObjectPtr<ABBBCharacter> Character = nullptr;
    /** 已成立的入包结果 */
    UPROPERTY()
    FBBBCharacterPickupReceipt Receipt;
};
