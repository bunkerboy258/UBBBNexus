#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/States/BBBFacingStates.h"
#include "BBBFacingRuntimeData.generated.h"

/** 角色朝向系统持续状态 */
USTRUCT(BlueprintType)
struct FBBBFacingRuntimeData
{
    GENERATED_BODY()

    /** @return 身体是否正在追赶瞄准方向 */
    bool IsBodyTurning() const
    {
        return State.bIsBodyTurning;
    }

    /** @return 当前水平瞄准偏角绝对值 */
    float GetCurrentAimYawAbs() const
    {
        return State.CurrentAimYawAbs;
    }

    /**
     * 提交当前朝向状态
     * @param bInIsBodyTurning	身体是否正在追赶瞄准方向
     * @param InCurrentAimYawAbs	当前水平瞄准偏角绝对值
     */
    void CommitState(bool bInIsBodyTurning, float InCurrentAimYawAbs)
    {
        State.bIsBodyTurning = bInIsBodyTurning;
        State.CurrentAimYawAbs = InCurrentAimYawAbs;
    }

private:
    /** 当前朝向状态 */
    UPROPERTY()
    FBBBCharacterFacingState State;
};
