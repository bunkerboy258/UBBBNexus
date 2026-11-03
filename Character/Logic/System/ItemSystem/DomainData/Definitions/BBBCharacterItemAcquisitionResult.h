#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Definitions/BBBCharacterItemAcquisitionKey.h"
#include "BBBCharacterItemAcquisitionResult.generated.h"

/** 本帧已完成的对应获得结果 */
USTRUCT()
struct FBBBCharacterItemAcquisitionResult final
{
    GENERATED_BODY()
    /** 本次获得的对应身份 */
    UPROPERTY()
    FBBBCharacterItemAcquisitionKey Key;
    /** 整次数量是否完整入包 */
    UPROPERTY()
    bool bSucceeded = false;
};
