
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Context/BBBAimStates.h"
#include "BBBAimRuntimeData.generated.h"
class FBBBAimRestoreProcessor;
class FBBBCharacterAimSystem;

USTRUCT(BlueprintType)
//瞄准状态
struct FBBBAimRuntimeData
{
    GENERATED_BODY()

    /**
     * 读取当前瞄准运行时状态
     * @return 瞄准运行时状态
     */
    const FBBBAimRuntimeState &GetState() const
    {
        return State;
    }

    /**
     * 读取瞄准起点的世界坐标
     * @return 瞄准起点世界坐标
     */
    const FVector &GetAimOriginWorld() const
    {
        return AimOriginWorld;
    }
private:
    friend class FBBBAimRestoreProcessor;
    friend class FBBBCharacterAimSystem;

    /**
     * 提交本地计算的瞄准状态与瞄准起点
     * @param InState	瞄准运行时状态
     * @param InAimOriginWorld	瞄准起点世界坐标
     */
    void CommitLocalState(
        const FBBBAimRuntimeState &InState,
        const FVector &InAimOriginWorld)
    {
        State = InState;
        AimOriginWorld = InAimOriginWorld;
    }

    /**
     * 用远端同步的瞄准状态还原本地运行时数据
     * @param RemoteState	远端传来的瞄准状态
     */
    void RestoreRemoteState(const FBBBRemoteAimState &RemoteState)
    {
        State.bIsAiming = RemoteState.bIsAiming;
        State.AimTargetWorld = RemoteState.AimTargetWorld;
        State.AimIKTargetWorld = RemoteState.AimTargetWorld;
    }

    UPROPERTY(Transient)
    FBBBAimRuntimeState State;

    UPROPERTY(Transient)
    FVector AimOriginWorld = FVector::ZeroVector;
};
