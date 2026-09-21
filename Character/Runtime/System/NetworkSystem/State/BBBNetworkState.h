#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkStates.h"
#include "BBBNetworkState.generated.h"

/** 角色网络领域唯一运行数据根 */
USTRUCT()
struct FBBBNetworkState
{
    GENERATED_BODY()

    FGuid LastUploadedEquipmentInstanceId;

    FBBBAimNetworkObserverState AimObserverState;

    FBBBLocomotionNetworkObserverState LocomotionObserverState;

};
