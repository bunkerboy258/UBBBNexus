#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkStates.h"
#include "BBBNetworkState.generated.h"

/** 角色网络领域唯一运行数据根 */
USTRUCT()
struct FBBBNetworkState
{
    GENERATED_BODY()

    /** @return 最后上传的主手装备实例标识 */
    const FGuid &GetLastUploadedEquipmentInstanceId() const
    {
        return LastUploadedEquipmentInstanceId;
    }

    /** @param EquipmentInstanceId 主手装备实例标识 */
    void CommitLastUploadedEquipmentInstanceId(const FGuid &EquipmentInstanceId)
    {
        LastUploadedEquipmentInstanceId = EquipmentInstanceId;
    }

    /** @return 瞄准上传观测状态 */
    const FBBBAimNetworkObserverState &GetAimObserverState() const
    {
        return AimObserverState;
    }

    /** @param ObserverState 瞄准上传观测状态 */
    void CommitAimObserverState(const FBBBAimNetworkObserverState &ObserverState)
    {
        AimObserverState = ObserverState;
    }

    /** @return 移动状态上传观测值 */
    const FBBBLocomotionNetworkObserverState &GetLocomotionObserverState() const
    {
        return LocomotionObserverState;
    }

    /** @param ObserverState 移动状态上传观测值 */
    void CommitLocomotionObserverState(const FBBBLocomotionNetworkObserverState &ObserverState)
    {
        LocomotionObserverState = ObserverState;
    }

private:
    FGuid LastUploadedEquipmentInstanceId;

    FBBBAimNetworkObserverState AimObserverState;

    FBBBLocomotionNetworkObserverState LocomotionObserverState;

};
