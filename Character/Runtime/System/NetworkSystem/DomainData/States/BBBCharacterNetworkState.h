#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkPayload.h"
#include "BBBCharacterNetworkState.generated.h"

/** 角色网络观察器需要跨帧保留的状态 */
USTRUCT()
struct FBBBCharacterNetworkState final
{
    GENERATED_BODY()

    /** 最近一次已经上传的装备实例标识 */
    FGuid LastUploadedEquipmentInstanceId;

    /** 最近一次已经上传的瞄准状态 */
    TOptional<FBBBAimNetworkPayload> LastObservedAim;

    /** 最近一次上传瞄准状态的世界时间 */
    float LastAimUploadTime = -1000.0f;

    /** 最近一次已经上传的移动状态 */
    TOptional<FBBBLocomotionNetworkPayload> LastObservedLocomotion;
};
