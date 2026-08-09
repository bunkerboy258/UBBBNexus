
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
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

private:
    friend class FBBBAimRestoreProcessor;
    friend class FBBBCharacterAimSystem;

    /**
     * 提交本地计算的瞄准状态
     * @param InState	瞄准运行时状态
     */
    void CommitLocalState(const FBBBAimRuntimeState &InState)
    {
        State = InState;
    }

    /**
     * 用远端同步的瞄准状态还原本地运行时数据
     * @param RemoteState	远端传来的瞄准状态
     */
    void RestoreRemoteState(const FBBBRemoteAimState &RemoteState)
    {
        State.bIsAiming = RemoteState.bIsAiming;
        State.bIsTurningInPlace = false;
        State.AimTargetWorld = RemoteState.AimTargetWorld;
    }

    UPROPERTY(Transient)
    FBBBAimRuntimeState State;

};
