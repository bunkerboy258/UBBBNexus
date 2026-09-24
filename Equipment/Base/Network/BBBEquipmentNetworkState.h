#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentNetworkState.generated.h"

/** 当前持有装备的一份完整传输状态 */
USTRUCT()
struct FBBBEquipmentNetworkState
{
    GENERATED_BODY()

    /** 装备定义 空标识表示空手 */
    UPROPERTY()
    FName EquipmentId = NAME_None;

    /** 持有关系发生变化时递增 */
    UPROPERTY()
    uint64 Generation = 0;

    /** 同一次持有期间状态发生变化时递增 */
    UPROPERTY()
    uint64 Revision = 0;

    /** 具体装备拥有并校验的状态内容 */
    UPROPERTY()
    TArray<uint8> Data;
};
