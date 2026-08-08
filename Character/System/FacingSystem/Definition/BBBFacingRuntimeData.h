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

    /** @return 相对角色前轴的水平瞄准偏角 */
    float GetAimYaw() const
    {
        return State.AimYaw;
    }

    /** @return 身体朝向计算使用的瞄准起点世界坐标 */
    const FVector &GetAimOriginWorld() const
    {
        return State.AimOriginWorld;
    }

    /**
     * 提交当前朝向状态
     * @param bInIsBodyTurning	身体是否正在追赶瞄准方向
     * @param InAimYaw		相对角色前轴的水平瞄准偏角
     * @param InAimOriginWorld	身体朝向计算使用的瞄准起点世界坐标
     */
    void CommitState(
        bool bInIsBodyTurning,
        float InAimYaw,
        const FVector &InAimOriginWorld)
    {
        State.bIsBodyTurning = bInIsBodyTurning;
        State.AimYaw = InAimYaw;
        State.AimOriginWorld = InAimOriginWorld;
    }

private:
    /** 当前朝向状态 */
    UPROPERTY()
    FBBBCharacterFacingState State;
};
