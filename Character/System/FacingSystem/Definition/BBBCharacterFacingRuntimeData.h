#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/States/BBBCharacterFacingStates.h"
#include "BBBCharacterFacingRuntimeData.generated.h"

class FBBBCharacterFacingSystem;

/**
 * 组装并保护角色朝向系统持有的跨帧状态
 */
USTRUCT()
struct FBBBCharacterFacingRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterFacingSystem;

    /** @return 朝向系统内部的只读滞回控制状态 */
    const FBBBCharacterFacingState &GetState() const
    {
        return State;
    }

    /**
     * 提交本帧计算完成的滞回控制状态
     * @param InState	本帧滞回控制状态
     */
    void CommitState(const FBBBCharacterFacingState &InState)
    {
        State = InState;
    }

    /** 仅供朝向系统维护的跨帧滞回控制状态 */
    UPROPERTY(Transient)
    FBBBCharacterFacingState State;
};
