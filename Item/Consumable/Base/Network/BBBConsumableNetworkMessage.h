#pragma once

#include "CoreMinimal.h"
#include "BBBConsumableNetworkMessage.generated.h"

/** 不解释消耗品业务的当前消息信封 */
USTRUCT()
struct FBBBConsumableNetworkMessage
{
    GENERATED_BODY()

    /** 目标消耗品标识 */
    UPROPERTY()
    FName ItemId;

    /** 本次持有实例的标识 */
    UPROPERTY()
    uint64 Generation = 0;

    /** 消息成立时的使用许可修订号 */
    UPROPERTY()
    uint64 UseRevision = 0;

    /** 同一次持有期间的消息顺序 */
    UPROPERTY()
    uint64 Revision = 0;

    /** 由具体消耗品定义的协议编号 */
    UPROPERTY()
    uint8 Kind = 0;

    /** 由具体消耗品编码与校验的内容 */
    UPROPERTY()
    TArray<uint8> Data;
};
