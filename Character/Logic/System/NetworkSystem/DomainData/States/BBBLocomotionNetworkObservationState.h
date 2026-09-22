#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBLocomotionNetworkObservationState.generated.h"

/** 移动网络观察处理器需要跨帧保留的状态 */
USTRUCT()
struct FBBBLocomotionNetworkObservationState final
{
    GENERATED_BODY()

    /** 最近一次已经上传的步态 */
    TOptional<EBBBCharacterGait> LastObservedGait;
};
