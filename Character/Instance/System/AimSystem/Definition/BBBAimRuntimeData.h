
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBAimRuntimeData.generated.h"

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
    friend class FBBBExecutionPipeline;
    friend class FBBBArbitrationPipeline;

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
    void ApplyRestoredState(const FBBBAimRuntimeState &RemoteState)
    {
        State.bIsAiming = RemoteState.bIsAiming;
        State.AimTargetWorld = RemoteState.AimTargetWorld;
    }

    UPROPERTY(Transient)
    FBBBAimRuntimeState State;

};
