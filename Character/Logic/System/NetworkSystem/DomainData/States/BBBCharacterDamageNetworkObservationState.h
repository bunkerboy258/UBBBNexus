#pragma once
#include "CoreMinimal.h"
class APawn;

/** 伤害单次转送的发送与接收基准 */
struct FBBBCharacterDamageNetworkObservationState final
{
    /** 已转送的本机命中批次 */
    uint64 DeliverySerial = 0;
    /** 同一来源在此角色生命周期中已经接收的最新序号 */
    TMap<TWeakObjectPtr<APawn>, uint64> ReceivedSequences;
};
