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

    /** @return 当前只读角色朝向状态 */
    const FBBBCharacterFacingState &GetState() const
    {
        return State;
    }

private:
    friend class FBBBCharacterFacingSystem;

    /**
     * 提交本帧计算完成的角色朝向状态
     * @param InState	本帧角色朝向状态
     */
    void CommitState(const FBBBCharacterFacingState &InState)
    {
        State = InState;
    }

    /** 角色朝向系统跨帧状态 */
    UPROPERTY(Transient)
    FBBBCharacterFacingState State;
};
