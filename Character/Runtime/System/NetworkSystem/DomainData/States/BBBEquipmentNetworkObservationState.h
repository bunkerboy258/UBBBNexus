#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentNetworkObservationState.generated.h"

/** 装备网络观察处理器需要跨帧保留的状态 */
USTRUCT()
struct FBBBEquipmentNetworkObservationState final
{
    GENERATED_BODY()

    /** 最近一次已经上传的装备实例标识 */
    FGuid LastUploadedEquipmentInstanceId;
};
